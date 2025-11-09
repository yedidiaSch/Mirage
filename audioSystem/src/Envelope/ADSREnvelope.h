#pragma once

/**
 * @file ADSREnvelope.h
 * @brief ADSR (Attack-Decay-Sustain-Release) envelope generator
 */

/**
 * @class ADSREnvelope
 * @brief ADSR envelope generator for amplitude modulation
 * 
 * Implements a classic ADSR envelope that shapes audio signals over time:
 * - Attack: Time to reach maximum amplitude when note starts
 * - Decay: Time to fall from peak to sustain level
 * - Sustain: Level maintained while note is held
 * - Release: Time to fade to silence when note ends
 * 
 * The envelope provides smooth amplitude changes to prevent clicks
 * and create natural-sounding note articulation.
 */
class ADSREnvelope {
public:
    /**
     * @brief Constructs an ADSR envelope with specified parameters
     * 
     * @param attackTime Time in seconds for attack phase
     * @param decayTime Time in seconds for decay phase  
     * @param sustainLevel Sustain amplitude level [0.0-1.0]
     * @param releaseTime Time in seconds for release phase
     */
    ADSREnvelope(float attackTime, float decayTime, float sustainLevel, float releaseTime);
    
    /**
     * @brief Process one sample through the envelope
     * 
     * @param noteOn Whether a note is currently being held
     * @param sampleRate Sample rate for time calculations
     * @return Current envelope amplitude [0.0-1.0]
     */
    float process(bool noteOn, float sampleRate);
    
    /**
     * @brief Reset the envelope to initial state
     */
    void reset();
    
private:
    /**
     * @enum Stage
     * @brief Current stage of the ADSR envelope
     */
    enum class Stage { 
        Attack,     ///< Rising to peak amplitude
        Decay,      ///< Falling to sustain level
        Sustain,    ///< Holding at sustain level
        Release,    ///< Fading to silence
        Idle        ///< No sound output
    };
    
    Stage currentStage;         ///< Current envelope stage
    
    float attackTime;           ///< Attack time in seconds
    float decayTime;            ///< Decay time in seconds
    float sustainLevel;         ///< Sustain amplitude level [0.0-1.0]
    float releaseTime;          ///< Release time in seconds
    float currentLevel;         ///< Current envelope output level
    float currentSample;        ///< Current sample count for timing
    float releaseStartLevel;    ///< Level captured when entering release
};