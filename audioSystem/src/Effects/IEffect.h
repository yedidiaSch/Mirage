#pragma once

#include <utility> // For std::pair

/**
 * @file IEffect.h
 * @brief Interface for audio effect processors
 */

/**
 * @interface IEffect
 * @brief Abstract interface for audio effects that process stereo samples
 * 
 * The IEffect interface defines a common protocol for classes that implement
 * audio effects (e.g., reverb, delay, filters, etc.). Effects implementing
 * this interface can be chained together and used interchangeably in audio
 * processing systems.
 * 
 * Effects should be designed to be stateless where possible, or provide proper
 * reset functionality for stateful effects.
 */
class IEffect 
{
public:
    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes
     */
    virtual ~IEffect() = default;

    /**
     * @brief Process a stereo audio sample
     * 
     * Takes an input stereo sample and applies the effect's processing to it.
     * The effect should not modify the input parameters and should return
     * a new processed sample.
     * 
     * @param stereoSample Input stereo sample (left and right channels)
     * @return Processed stereo sample after applying the effect
     * 
     * @note Input sample values are typically in the range [-1.0, 1.0]
     * @note Output should also be in the range [-1.0, 1.0] to prevent clipping
     */
    virtual std::pair<float, float> process(std::pair<float, float> stereoSample) = 0;

    /**
     * @brief Reset the effect's internal state
     * 
     * Resets any internal state variables to their initial values.
     * This method should be called when starting a new audio stream or
     * when discontinuities in the audio signal occur.
     * 
     * For stateless effects, this method may have no implementation.
     * For stateful effects (e.g., delay, reverb), this should clear
     * internal buffers and reset parameters to default states.
     */
    virtual void reset() {}
};