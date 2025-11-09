#pragma once

#include "IEffect.h"

/**
 * @file OctaveEffect.h
 * @brief Octave doubling/halving effect implementation
 */

/**
 * @class OctaveEffect
 * @brief Audio effect that adds octave harmonics to the input signal
 * 
 * This effect generates an octave-shifted version of the input signal and blends
 * it with the original. It can produce either higher octave (frequency doubling)
 * or lower octave (frequency halving) effects, commonly used in guitar pedals
 * and synthesizers.
 * 
 * The effect works by tracking the input signal's frequency and generating
 * a synthetic waveform at the desired octave interval.
 */
class OctaveEffect : public IEffect 
{
public:
    /**
     * @brief Constructs an OctaveEffect with specified parameters
     * 
     * @param higher Whether to generate higher (true) or lower (false) octave
     * @param blend Blending factor between original and octave signal [0.0-1.0]
     *              0.0 = original only, 1.0 = octave only, 0.5 = equal mix
     */
    OctaveEffect(bool higher = true, float blend = 0.5f);

    /**
     * @brief Process a stereo sample with octave effect
     * 
     * @param stereoSample Input stereo sample to process
     * @return Processed sample with octave effect applied
     */
    std::pair<float, float> process(std::pair<float, float> stereoSample) override;
    
    /**
     * @brief Reset the internal phase and state
     */
    void reset() override;

    /**
     * @brief Set whether to generate higher or lower octave
     * @param higher true for higher octave, false for lower octave
     */
    void setHigher(bool higher);
    
    /**
     * @brief Set the blend ratio between original and octave signals
     * @param blend Blend factor [0.0-1.0]
     */
    void setBlend(float blend);
    
    /**
     * @brief Set the fundamental frequency for octave generation
     * @param frequency Fundamental frequency in Hz
     */
    void setFrequency(float frequency);
    
    /**
     * @brief Set the sample rate for proper phase calculation
     * @param sampleRate Sample rate in Hz
     */
    void setSampleRate(float sampleRate);

private:
    bool m_higher;          ///< Whether the effect generates higher or lower octave
    float m_blend;          ///< Blending factor between original and octave sample [0.0-1.0]
    float m_phase;          ///< Phase accumulator for generating the octave wave [0.0-1.0)
    float m_frequency;      ///< Fundamental frequency in Hz
    float m_sampleRate;     ///< Sample rate in Hz
    float m_stateL;         ///< Filter state for left channel
    float m_stateR;         ///< Filter state for right channel
};
