#pragma once

#include <string>
#include <vector>

/**
 * @file AudioConfig.h
 * @brief Configuration structure for the audio system
 */

/**
 * @brief Configuration options for selecting waveform and effects
 */
struct AudioConfig
{
    std::string waveform;               ///< Name of the oscillator to use
    std::vector<std::string> effects;   ///< Ordered list of effect names
    float sampleRate;                   ///< Audio sample rate in Hz
    unsigned int bufferFrames;          ///< Number of frames per audio buffer
    int midiPort;                       ///< MIDI port number
    float defaultFrequency;             ///< Default frequency for testing (Hz)
    std::string inputMode;              ///< Input mode: "midi" or "sequencer" for testing
    std::string sequenceType;           ///< Type of sequence for sequencer mode
    
    // ADSR envelope parameters
    float attackTime;                   ///< ADSR attack time in seconds
    float decayTime;                    ///< ADSR decay time in seconds
    float sustainLevel;                 ///< ADSR sustain level [0.0-1.0]
    float releaseTime;                  ///< ADSR release time in seconds
    
    // Default constructor with sensible defaults
    AudioConfig() : 
        waveform("sine"),
        sampleRate(44100.0f),
        bufferFrames(512),
        midiPort(1),
        defaultFrequency(440.0f),
        inputMode("midi"),
        sequenceType("demo"),
        attackTime(0.1f),
        decayTime(0.2f),
        sustainLevel(0.7f),
        releaseTime(0.3f)
    {}
};
