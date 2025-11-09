#pragma once
#include "IWave.h"

/**
 * @brief Sine wave generator implementation
 * 
 * Generates a smooth sine wave with amplitude between -1.0 and 1.0.
 * A sine wave is the purest tonal representation with a single frequency component.
 * 
 * Properties:
 * - Contains only the fundamental frequency (no harmonics)
 * - Produces the smoothest possible waveform with no aliasing
 * - Mathematically described by y = sin(2π * phase)
 * - Computationally more expensive than other waveforms due to trigonometric calculations
 * 
 * Common applications:
 * - Pure tones for testing and calibration
 * - Fundamental building block for additive synthesis
 * - Base carrier for FM synthesis
 * - Natural sounding tones and soft sounds
 */
class SineWave : public IWave 
{
public:
    /**
     * @brief Constructs a new Sine Wave object
     * 
     * Initializes the sine wave generator with default settings.
     */
    SineWave();
    
    /**
     * @brief Destroys the Sine Wave object
     * 
     * Virtual destructor to ensure proper cleanup of derived classes.
     */
    ~SineWave() override;

    /**
     * @brief Generate a sine wave sample
     * 
     * Produces a sample value for a sine wave at the specified frequency.
     * Uses the standard sine function with the phase mapped to a full
     * 2π cycle to produce a smooth waveform.
     * 
     * @param frequency The frequency of the waveform in Hertz (Hz)
     * @param sampleRate The sample rate of the audio system in samples per second (Hz)
     * @param phase Reference to the current phase of the waveform (0.0 to 1.0)
     * @return float Sample value between -1.0 and 1.0
     * 
     * @note The phase is automatically wrapped to the range [0, 1) if it exceeds 1.0
     * @see IWave::generate for the interface contract
     */
    float generate(float frequency, float sampleRate, float& phase) override;

    /**
     * @brief Reset the wave generator state
     * 
     * Sine wave generation is stateless, so this method has no effect.
     * Implemented for consistency with the IWave interface.
     * 
     * @note This does not reset the phase variable passed to generate(),
     *       as that is managed by the caller
     */
    void reset() override;
};
