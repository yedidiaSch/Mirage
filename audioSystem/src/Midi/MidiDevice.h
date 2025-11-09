#pragma once

#include "RtMidi.h"
#include "subject.h"
#include "MidiEvent.h"
#include <vector>
#include <string>

/**
 * @class MidiDevice
 * @brief Manages MIDI input device connections and processes incoming MIDI messages.
 *
 * This class provides an interface for connecting to MIDI input devices,
 * receiving MIDI messages, and converting them into appropriate events
 * that can be observed by other components in the system. It inherits from
 * Subject to implement the Observer pattern for event notification.
 */
class MidiDevice : public Subject {
public:
    /**
     * @brief Constructor with optional port selection
     * @param portNumber The MIDI port number to connect to (default: 0)
     */
    MidiDevice(int portNumber = 0);
    
    /**
     * @brief Destructor
     * Closes MIDI connections and cleans up resources
     */
    ~MidiDevice();
    
    /**
     * @brief Starts listening for MIDI input
     * Activates the MIDI input callback to begin processing incoming MIDI messages
     */
    void start();
    
    /**
     * @brief Stops listening for MIDI input
     * Deactivates the MIDI input callback to stop processing incoming MIDI messages
     */
    void stop();
    
    /**
     * @brief Lists all available MIDI input ports
     * @return Vector of strings containing the names of available MIDI input ports
     */
    std::vector<std::string> listPorts();
    
    /**
     * @brief Changes the active MIDI input port
     * @param portNumber Index of the port to connect to
     * @return true if port change was successful, false otherwise
     */
    bool changePort(unsigned int portNumber);
    
private:
    /**
     * @brief RtMidi input object for handling MIDI communication
     */
    RtMidiIn midiin;
    
    /**
     * @brief Indicates whether the MIDI device is properly initialized
     */
    bool isInitialized;
    
    /**
     * @brief Static callback function for MIDI messages
     * 
     * This function is called by RtMidi when a MIDI message arrives.
     * It then dispatches the message to the appropriate handler.
     * 
     * @param timeStamp The time when the message was received
     * @param message Pointer to the MIDI message data
     * @param userData Pointer to the MidiDevice instance
     */
    static void midiCallback(double timeStamp, std::vector<unsigned char>* message, void* userData);
    
    /**
     * @brief Processes Note On MIDI messages
     * @param channel MIDI channel (0-15)
     * @param note MIDI note number (0-127)
     * @param velocity Note velocity/intensity (0-127)
     */
    void handleNoteOn(unsigned char channel, unsigned char note, unsigned char velocity);
    
    /**
     * @brief Processes Note Off MIDI messages
     * @param channel MIDI channel (0-15)
     * @param note MIDI note number (0-127)
     */
    void handleNoteOff(unsigned char channel, unsigned char note);
    
    /**
     * @brief Processes Control Change MIDI messages
     * @param channel MIDI channel (0-15)
     * @param controller Controller number (0-127)
     * @param value Controller value (0-127)
     */
    void handleControlChange(unsigned char channel, unsigned char controller, unsigned char value);
    
    /**
     * @brief Processes Pitch Bend MIDI messages
     * @param channel MIDI channel (0-15)
     * @param value Pitch bend value (-8192 to 8191)
     */
    void handlePitchBend(unsigned char channel, int value);
    
    /**
     * @brief Converts a MIDI note number to its corresponding frequency in Hz
     * 
     * Uses the standard formula: f = 440 × 2^((n-69)/12)
     * where n is the MIDI note number
     * 
     * @param midiNote MIDI note number (0-127)
     * @return The frequency in Hz corresponding to the MIDI note
     */
    static float midiNoteToFrequency(unsigned char midiNote);
};