#pragma once
#include "IWave.h"

/**
 * @brief Triangle wave generator
 *
 * Generates a symmetrical triangular waveform, which contains only odd
 * harmonics with amplitudes decreasing by the square of the harmonic number.
 */
class TriangleWave : public IWave
{
public:
    /// Construct a TriangleWave generator
    TriangleWave();
    /// Virtual destructor
    ~TriangleWave() override;

    /// Generate the next sample of the triangle
    float generate(float frequency, float sampleRate, float& phase) override;
    /// Reset internal state (no-op for triangle)
    void reset() override;
};
