#include "ConfigReader.h"
#include <stdexcept>
#include <iostream>
#include <cstring>

ConfigReader::ConfigReader()
{
    // Initialize libxml2
    xmlInitParser();
    LIBXML_TEST_VERSION
}

ConfigReader::~ConfigReader()
{
    // Cleanup libxml2
    xmlCleanupParser();
}

AudioConfig ConfigReader::loadConfig(const std::string& filename)
{
    AudioConfig config; // Default values will be set by constructor
    
    // Parse the XML file
    xmlDoc* doc = xmlReadFile(filename.c_str(), NULL, 0);
    if (doc == NULL) {
        throw std::runtime_error("Failed to parse XML file: " + filename);
    }
    
    // Get the root element
    xmlNode* root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        xmlFreeDoc(doc);
        throw std::runtime_error("Empty XML document: " + filename);
    }
    
    // Check if root element is correct
    if (strcmp((const char*)root->name, "audioSystemConfig") != 0) {
        xmlFreeDoc(doc);
        throw std::runtime_error("Invalid root element in XML file. Expected 'audioSystemConfig'");
    }
    
    // Parse configuration sections
    for (xmlNode* node = root->children; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) continue;
        
        std::string nodeName = (const char*)node->name;
        
        if (nodeName == "audio") {
            // Parse audio configuration
            xmlNode* sampleRateNode = findChildNode(node, "sampleRate");
            if (sampleRateNode) {
                config.sampleRate = getNodeFloat(sampleRateNode, config.sampleRate);
            }
            
            xmlNode* bufferFramesNode = findChildNode(node, "bufferFrames");
            if (bufferFramesNode) {
                config.bufferFrames = getNodeInt(bufferFramesNode, config.bufferFrames);
            }
        }
        else if (nodeName == "waveform") {
            // Parse waveform configuration
            xmlNode* typeNode = findChildNode(node, "type");
            if (typeNode) {
                config.waveform = getNodeText(typeNode);
            }
        }
        else if (nodeName == "effects") {
            // Parse effects configuration
            config.effects.clear(); // Clear default effects
            for (xmlNode* effectNode = node->children; effectNode; effectNode = effectNode->next) {
                if (effectNode->type == XML_ELEMENT_NODE && 
                    strcmp((const char*)effectNode->name, "effect") == 0) {
                    std::string effectName = getNodeText(effectNode);
                    if (!effectName.empty()) {
                        config.effects.push_back(effectName);
                    }
                }
            }
        }
        else if (nodeName == "midi") {
            // Parse MIDI configuration
            xmlNode* portNode = findChildNode(node, "port");
            if (portNode) {
                config.midiPort = getNodeInt(portNode, config.midiPort);
            }
        }
        else if (nodeName == "defaultFrequency") {
            // Parse default frequency
            config.defaultFrequency = getNodeFloat(node, config.defaultFrequency);
        }
        else if (nodeName == "input") {
            // Parse input configuration
            xmlNode* modeNode = findChildNode(node, "mode");
            if (modeNode) {
                config.inputMode = getNodeText(modeNode);
            }
            
            xmlNode* sequenceTypeNode = findChildNode(node, "sequenceType");
            if (sequenceTypeNode) {
                config.sequenceType = getNodeText(sequenceTypeNode);
            }
        }
        else if (nodeName == "envelope") {
            // Parse ADSR envelope configuration
            xmlNode* attackNode = findChildNode(node, "attack");
            if (attackNode) {
                config.attackTime = getNodeFloat(attackNode, config.attackTime);
            }
            
            xmlNode* decayNode = findChildNode(node, "decay");
            if (decayNode) {
                config.decayTime = getNodeFloat(decayNode, config.decayTime);
            }
            
            xmlNode* sustainNode = findChildNode(node, "sustain");
            if (sustainNode) {
                config.sustainLevel = getNodeFloat(sustainNode, config.sustainLevel);
            }
            
            xmlNode* releaseNode = findChildNode(node, "release");
            if (releaseNode) {
                config.releaseTime = getNodeFloat(releaseNode, config.releaseTime);
            }
        }
    }
    
    xmlFreeDoc(doc);
    return config;
}

AudioConfig ConfigReader::loadConfigWithFallback(const std::string& filename)
{
    AudioConfig config; // Start with defaults
    
    try {
        config = loadConfig(filename);
        std::cout << "✓ Successfully loaded configuration from " << filename << std::endl;
        printConfig(config, filename);
    } catch (const std::exception& e) {
        std::cout << "⚠ Warning: Failed to load " << filename << " (" << e.what() << ")" << std::endl;
        std::cout << "Using default configuration values." << std::endl;
        printConfig(config, "defaults");
    }
    
    return config;
}

void ConfigReader::printConfig(const AudioConfig& config, const std::string& source)
{
    if (!source.empty()) {
        std::cout << "\n--- Configuration (" << source << ") ---" << std::endl;
    } else {
        std::cout << "\n--- Configuration ---" << std::endl;
    }
    
    std::cout << "  Waveform: " << config.waveform << std::endl;
    std::cout << "  Sample Rate: " << config.sampleRate << " Hz" << std::endl;
    std::cout << "  Buffer Frames: " << config.bufferFrames << std::endl;
    std::cout << "  Input Mode: " << config.inputMode << std::endl;
    
    if (config.inputMode == "midi") {
        std::cout << "  MIDI Port: " << config.midiPort << std::endl;
    } else if (config.inputMode == "sequencer") {
        std::cout << "  Sequence Type: " << config.sequenceType << std::endl;
    }
    
    std::cout << "  Default Frequency: " << config.defaultFrequency << " Hz" << std::endl;
    std::cout << "  Effects: ";
    
    if (config.effects.empty()) {
        std::cout << "(none)";
    } else {
        for (size_t i = 0; i < config.effects.size(); ++i) {
            std::cout << config.effects[i];
            if (i < config.effects.size() - 1) std::cout << ", ";
        }
    }
    std::cout << std::endl;
    std::cout << "  ADSR Envelope:" << std::endl;
    std::cout << "    Attack:  " << config.attackTime << " s" << std::endl;
    std::cout << "    Decay:   " << config.decayTime << " s" << std::endl;
    std::cout << "    Sustain: " << config.sustainLevel << std::endl;
    std::cout << "    Release: " << config.releaseTime << " s" << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

std::string ConfigReader::getNodeText(xmlNode* node)
{
    if (node == NULL) return "";
    
    xmlChar* content = xmlNodeGetContent(node);
    if (content == NULL) return "";
    
    std::string result = (const char*)content;
    xmlFree(content);
    return result;
}

float ConfigReader::getNodeFloat(xmlNode* node, float defaultValue)
{
    std::string text = getNodeText(node);
    if (text.empty()) return defaultValue;
    
    try {
        return std::stof(text);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

int ConfigReader::getNodeInt(xmlNode* node, int defaultValue)
{
    std::string text = getNodeText(node);
    if (text.empty()) return defaultValue;
    
    try {
        return std::stoi(text);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

xmlNode* ConfigReader::findChildNode(xmlNode* parent, const std::string& name)
{
    if (parent == NULL) return NULL;
    
    for (xmlNode* child = parent->children; child; child = child->next) {
        if (child->type == XML_ELEMENT_NODE && 
            strcmp((const char*)child->name, name.c_str()) == 0) {
            return child;
        }
    }
    return NULL;
}