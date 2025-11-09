#include "MidiDevice.h"
#include "MidiEvent.h"
#include <iostream>
#include "notes.h"

// Constructor with port selection
MidiDevice::MidiDevice(int portNumber) : isInitialized(false) {
    try {
        // List available ports
        unsigned int nPorts = midiin.getPortCount();
        if (nPorts == 0) {
            std::cerr << "No MIDI input ports available!" << std::endl;
            return;
        }
        
        // Use specified port or default to 0
        int portToOpen = (portNumber >= 0 && portNumber < static_cast<int>(nPorts)) ? portNumber : 0;
        
        std::cout << "Opening MIDI port: " << midiin.getPortName(portToOpen) << std::endl;
        midiin.openPort(portToOpen);
        midiin.setCallback(&MidiDevice::midiCallback, this);
        midiin.ignoreTypes(false, false, false);
        isInitialized = true;
        std::cout << "MIDI device initialized successfully." << std::endl;
    } catch (RtMidiError& error) {
        std::cerr << "Error initializing MIDI device: " << error.getMessage() << std::endl;
    }
}

// Destructor
MidiDevice::~MidiDevice() {
    stop();
}

// Start receiving MIDI messages
void MidiDevice::start() {
    if (isInitialized) {
        std::cout << "MIDI device started." << std::endl;
    } else {
        std::cerr << "Cannot start MIDI device: not properly initialized." << std::endl;
    }
}

// Stop receiving MIDI messages
void MidiDevice::stop() {
    if (midiin.isPortOpen()) {
        midiin.closePort();
        std::cout << "MIDI device stopped." << std::endl;
    }
}

// List available MIDI ports
std::vector<std::string> MidiDevice::listPorts() {
    std::vector<std::string> portNames;
    unsigned int nPorts = midiin.getPortCount();
    
    std::cout << "Available MIDI input ports:" << std::endl;
    for (unsigned int i = 0; i < nPorts; i++) {
        std::string portName = midiin.getPortName(i);
        portNames.push_back(portName);
        std::cout << "  [" << i << "] " << portName << std::endl;
    }
    
    return portNames;
}

// Change MIDI input port
bool MidiDevice::changePort(unsigned int portNumber) {
    if (midiin.isPortOpen()) {
        midiin.closePort();
    }
    
    try {
        if (portNumber >= midiin.getPortCount()) {
            std::cerr << "Port number out of range." << std::endl;
            return false;
        }
        
        midiin.openPort(portNumber);
        std::cout << "Changed to MIDI port: " << midiin.getPortName(portNumber) << std::endl;
        isInitialized = true;
        return true;
    } catch (RtMidiError& error) {
        std::cerr << "Error changing MIDI port: " << error.getMessage() << std::endl;
        isInitialized = false;
        return false;
    }
}

// Static callback function for MIDI messages
void MidiDevice::midiCallback(double timeStamp, std::vector<unsigned char>* message, void* userData) {
    (void)timeStamp;
    
    if (message->empty()) return;

    MidiDevice* device = static_cast<MidiDevice*>(userData);
    unsigned char status = message->at(0);
    unsigned char channel = status & 0x0F;
    unsigned char messageType = status & 0xF0;
    unsigned char data1 = message->size() > 1 ? message->at(1) : 0;
    unsigned char data2 = message->size() > 2 ? message->at(2) : 0;

    // Handle different MIDI message types
    switch (messageType) {
        case 0x80: // Note Off
            device->handleNoteOff(channel, data1);
            break;
            
        case 0x90: // Note On
            if (data2 > 0) {
                device->handleNoteOn(channel, data1, data2);
            } else {
                device->handleNoteOff(channel, data1); // Note On with velocity 0 is equivalent to Note Off
            }
            break;
            
        case 0xB0: // Control Change
            device->handleControlChange(channel, data1, data2);
            break;
            
        case 0xE0: // Pitch Bend
            // Combine two 7-bit values into one 14-bit value
            device->handlePitchBend(channel, ((data2 << 7) | data1) - 8192); // Center at 0
            break;
            
        // Add more message types as needed
    }
}

// Handle MIDI Note On messages
void MidiDevice::handleNoteOn(unsigned char channel, unsigned char note, unsigned char velocity) {
    MidiEvent event;
    event.type = MidiEventType::NOTE_ON;
    event.channel = channel;
    event.data1 = note;
    event.data2 = velocity;
    event.value = midiNoteToFrequency(note);  // Convert note to frequency
    
    // Notify all observers about the Note On event
    notify(&event);
}

// Handle MIDI Note Off messages
void MidiDevice::handleNoteOff(unsigned char channel, unsigned char note) {
    MidiEvent event;
    event.type = MidiEventType::NOTE_OFF;
    event.channel = channel;
    event.data1 = note;
    event.data2 = 0;
    event.value = midiNoteToFrequency(note);
    
    // Notify all observers about the Note Off event
    notify(&event);
}

// Handle MIDI Control Change messages
void MidiDevice::handleControlChange(unsigned char channel, unsigned char controller, unsigned char value) {
    MidiEvent event;
    event.type = MidiEventType::CONTROL_CHANGE;
    event.channel = channel;
    event.data1 = controller;
    event.data2 = value;
    
    // Notify all observers about the Control Change event
    notify(&event);
    
    // Log the event (can be removed if not needed)
    std::cout << "Control Change: " << static_cast<int>(controller) 
              << " Value: " << static_cast<int>(value) << std::endl;
}

// Handle MIDI Pitch Bend messages
void MidiDevice::handlePitchBend(unsigned char channel, int value) {
    MidiEvent event;
    event.type = MidiEventType::PITCH_BEND;
    event.channel = channel;
    event.value = value;
    
    // Notify all observers about the Pitch Bend event
    notify(&event);
    
    // Log the event (can be removed if not needed)
    std::cout << "Pitch bend: " << value << std::endl;
}

// Helper function to convert MIDI note number to frequency
float MidiDevice::midiNoteToFrequency(unsigned char midiNote) {
    return MIDI_NOTE_FREQUENCIES[midiNote];
}
