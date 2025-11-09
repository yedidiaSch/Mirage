#pragma once

/**
 * @brief Interface for audio waveform generators
 *
 * The IWave interface defines a common protocol for classes that generate
 * different types of audio waveforms (e.g., sine, square, sawtooth, etc.).
 * Classes implementing this interface can be used interchangeably in audio
 * processing systems.
 */
class IWave 
{

public:

    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes
     */
    virtual ~IWave() = default;

    /**
     * @brief Generate the next sample for the waveform
     *
     * @param frequency The frequency of the waveform in Hertz
     * @param sampleRate The sample rate of the audio system in samples per second
     * @param phase Reference to the current phase of the waveform, which will be updated
     *              Phase is typically in the range [0, 1) representing one complete cycle
     *
     * @return The calculated sample value, typically in the range [-1.0, 1.0]
     */
    virtual float generate(float frequency, float sampleRate, float& phase) = 0;

    /**
     * @brief Reset the state of the waveform generator
     * 
     * This method should reset any internal state variables to their initial values,
     * effectively restarting the waveform generation from its beginning state.
     */
    virtual void reset() = 0;
    
};
