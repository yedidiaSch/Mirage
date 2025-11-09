#pragma once

#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "AudioConfig.h"

/**
 * @file ConfigReader.h
 * @brief XML configuration file reader for audio system
 */

/**
 * @class ConfigReader
 * @brief Reads audio system configuration from XML files
 * 
 * This class provides functionality to parse XML configuration files
 * and populate AudioConfig structures with the specified values.
 */
class ConfigReader
{
public:
    /**
     * @brief Constructor
     */
    ConfigReader();
    
    /**
     * @brief Destructor - cleans up libxml2 resources
     */
    ~ConfigReader();
    
    /**
     * @brief Load configuration from XML file
     * @param filename Path to the XML configuration file
     * @return AudioConfig structure populated with values from XML
     * @throws std::runtime_error if file cannot be read or parsed
     */
    AudioConfig loadConfig(const std::string& filename);

    /**
     * @brief Load configuration with automatic fallback to defaults and detailed logging
     * @param filename Path to the XML configuration file
     * @return AudioConfig structure, either from XML or with default values
     */
    AudioConfig loadConfigWithFallback(const std::string& filename);

    /**
     * @brief Print configuration details to console
     * @param config The configuration to print
     * @param source Description of the configuration source (e.g., "config.xml", "defaults")
     */
    static void printConfig(const AudioConfig& config, const std::string& source = "");

private:
    /**
     * @brief Parse a text node and return its content as string
     * @param node XML node to parse
     * @return String content of the node, or empty string if invalid
     */
    std::string getNodeText(xmlNode* node);
    
    /**
     * @brief Parse a text node and return its content as float
     * @param node XML node to parse
     * @param defaultValue Default value to return if parsing fails
     * @return Float value parsed from node content
     */
    float getNodeFloat(xmlNode* node, float defaultValue = 0.0f);
    
    /**
     * @brief Parse a text node and return its content as integer
     * @param node XML node to parse
     * @param defaultValue Default value to return if parsing fails
     * @return Integer value parsed from node content
     */
    int getNodeInt(xmlNode* node, int defaultValue = 0);
    
    /**
     * @brief Find a child node by name
     * @param parent Parent node to search in
     * @param name Name of the child node to find
     * @return Pointer to the found node, or nullptr if not found
     */
    xmlNode* findChildNode(xmlNode* parent, const std::string& name);
};