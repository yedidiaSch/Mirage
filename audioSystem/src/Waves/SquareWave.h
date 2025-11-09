#pragma once
#include "IWave.h"

/**
 * @brief Square wave generator implementation
 * 
 * Generates a square wave with amplitude alternating between -1.0 and 1.0.
 * A square wave is characterized by abrupt transitions between two fixed values,
 * creating a signal that resembles a square pattern when visualized.
 * 
 * Properties:
 * - Contains odd harmonics (f, 3f, 5f, 7f, etc.) with amplitudes decreasing by 1/n
 * - Rich in high-frequency content, useful for synthesis and modulation
 * - Standard implementation uses 50% duty cycle (equal time at high and low states)
 * - Computationally efficient compared to sine waves
 * 
 * Common applications:
 * - Synthesizers (especially for bass and lead sounds)
 * - Digital clock signals
 * - Pulse Width Modulation (PWM) basis
 */
class SquareWave : public IWave 
{
public:
    /**
     * @brief Constructs a new Square Wave object
     * 
     * Initializes the square wave generator with default settings.
     * No parameters needed as the square wave alternates between -1.0 and 1.0
     * with a fixed 50% duty cycle.
     */
    SquareWave();
    
    /**
     * @brief Destroys the Square Wave object
     * 
     * Virtual destructor to ensure proper cleanup of derived classes.
     */
    ~SquareWave() override;

    /**
     * @brief Generate a square wave sample
     * 
     * Produces a sample value for a square wave at the specified frequency.
     * The implementation uses phase accumulation to track the wave position
     * and determines the output value based on whether the phase is in the
     * first or second half of the cycle.
     * 
     * @param frequency The frequency of the waveform in Hertz (Hz)
     * @param sampleRate The sample rate of the audio system in samples per second (Hz)
     * @param phase Reference to the current phase of the waveform (0.0 to 1.0)
     * @return float Sample value of either -1.0 or 1.0
     * 
     * @note The phase is automatically wrapped to the range [0, 1) if it exceeds 1.0
     * @see IWave::generate for the interface contract
     */
    float generate(float frequency, float sampleRate, float& phase) override;

    /**
     * @brief Reset the wave generator state
     * 
     * Resets any internal state variables to their default values.
     * For the basic square wave, this has minimal effect since the output
     * depends only on the current phase position, but is implemented for
     * consistency with the IWave interface.
     * 
     * @note This does not reset the phase variable passed to generate(),
     *       as that is managed by the caller
     */
    void reset() override;
};