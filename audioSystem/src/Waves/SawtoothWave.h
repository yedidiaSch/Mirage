#pragma once
#include "IWave.h"

/**
 * @brief Sawtooth wave generator
 *
 * Produces a ramp waveform that rises linearly and then drops sharply.
 * The sawtooth contains both even and odd harmonics, giving it a bright
 * timbre often used in subtractive synthesis.
 */
class SawtoothWave : public IWave
{
public:
    /// Construct a SawtoothWave generator
    SawtoothWave();
    /// Virtual destructor
    ~SawtoothWave() override;

    /// Generate the next sample of the sawtooth
    float generate(float frequency, float sampleRate, float& phase) override;
    /// Reset internal state (no-op for sawtooth)
    void reset() override;
};
