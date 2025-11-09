#include "TriangleWave.h"
#include <cmath>

// -----------------------------------------------------------------------------
// TriangleWave implementation
// -----------------------------------------------------------------------------

TriangleWave::TriangleWave() {}
TriangleWave::~TriangleWave() {}

float TriangleWave::generate(float frequency, float sampleRate, float& phase)
{
    // Input validation
    if (frequency <= 0.0f || sampleRate <= 0.0f) {
        return 0.0f;
    }
    
    // Piecewise linear ramp creating a symmetric triangle
    // First half: ramp up from -1 to 1
    // Second half: ramp down from 1 to -1
    float sample = phase < 0.5f ? 4.0f * phase - 1.0f : -4.0f * phase + 3.0f;
    
    // Update phase
    phase += frequency / sampleRate;
    
    // Robust phase wrapping
    if (phase >= 1.0f) {
        phase = std::fmod(phase, 1.0f);
    }
    
    return sample;
}

void TriangleWave::reset()
{
    // no state to reset
}
