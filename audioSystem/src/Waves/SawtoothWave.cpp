#include "SawtoothWave.h"
#include <cmath>

// -----------------------------------------------------------------------------
// SawtoothWave implementation
// -----------------------------------------------------------------------------

SawtoothWave::SawtoothWave() {}
SawtoothWave::~SawtoothWave() {}

float SawtoothWave::generate(float frequency, float sampleRate, float& phase)
{
    // Input validation
    if (frequency <= 0.0f || sampleRate <= 0.0f) {
        return 0.0f;
    }
    
    // Sawtooth is a simple ramp from -1 to 1
    float sample = 2.0f * phase - 1.0f;
    
    // Update phase
    phase += frequency / sampleRate;
    
    // Robust phase wrapping
    if (phase >= 1.0f) {
        phase = std::fmod(phase, 1.0f);
    }
    
    return sample;
}

void SawtoothWave::reset()
{
    // no state to reset
}
