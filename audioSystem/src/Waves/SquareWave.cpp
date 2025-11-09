#include "SquareWave.h"
#include <cmath>

SquareWave::SquareWave()
{
    // Initialize any member variables here if needed
}

SquareWave::~SquareWave()
{
    // Cleanup resources if needed
}

float SquareWave::generate(float frequency, float sampleRate, float& phase)
{
    // Input validation
    if (frequency <= 0.0f || sampleRate <= 0.0f) {
        return 0.0f;
    }
    
    // Generate a square wave sample
    float sample = (phase < 0.5f) ? 1.0f : -1.0f;

    // Update the phase for the square wave
    phase += frequency / sampleRate;
    
    // Robust phase wrapping
    if (phase >= 1.0f) {
        phase = std::fmod(phase, 1.0f);
    }

    return sample;
}

void SquareWave::reset()
{
    // No state to reset for a simple square wave
    // If we add state variables later, reset them here
}