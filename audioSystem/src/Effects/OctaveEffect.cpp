#include "OctaveEffect.h"
#include <cmath>
#include <algorithm>

OctaveEffect::OctaveEffect(bool higher, float blend)
    : m_higher(higher), 
      m_blend(std::min(std::max(blend, 0.0f), 1.0f)), 
      m_phase(0.0f), 
      m_frequency(0.0f), 
      m_sampleRate(44100.0f), // Default sample rate
      m_stateL(0.0f),
      m_stateR(0.0f)
{
}

std::pair<float, float> OctaveEffect::process(std::pair<float, float> stereoSample) 
{
    // Simple approach: Just apply waveshaping/filtering for harmonic content
    // This won't be a true pitch-shifted octave but will add harmonic richness
    // A real octave effect requires complex pitch shifting algorithms (FFT, granular synthesis)
    
    if (m_blend <= 0.0f) {
        return stereoSample; // No effect
    }
    
    // Simple processing for harmonic generation
    float left = stereoSample.first;
    float right = stereoSample.second;
    
    if (m_higher) {
        // Add upper harmonics by waveshaping (soft clipping adds odd harmonics)
        float shapedLeft = std::tanh(left * 2.0f) * 0.8f;
        float shapedRight = std::tanh(right * 2.0f) * 0.8f;
        
        left = (1.0f - m_blend) * left + m_blend * shapedLeft;
        right = (1.0f - m_blend) * right + m_blend * shapedRight;
    } else {
        // For lower octave, apply simple low-pass filtering
        // This is a very basic one-pole filter
        m_stateL = m_stateL * 0.8f + left * 0.2f;
        m_stateR = m_stateR * 0.8f + right * 0.2f;
        
        left = (1.0f - m_blend) * left + m_blend * m_stateL;
        right = (1.0f - m_blend) * right + m_blend * m_stateR;
    }
    
    return {left, right};
}

void OctaveEffect::reset() 
{
    m_phase = 0.0f;
    m_stateL = 0.0f;
    m_stateR = 0.0f;
}

void OctaveEffect::setHigher(bool higher) 
{
    m_higher = higher;
}

void OctaveEffect::setBlend(float blend) 
{
    m_blend = std::min(std::max(blend, 0.0f), 1.0f);
}

void OctaveEffect::setFrequency(float frequency) 
{
    // Validate frequency range
    if (frequency > 0.0f && frequency <= 20000.0f) {
        m_frequency = frequency;
    }
}

void OctaveEffect::setSampleRate(float sampleRate) 
{
    // Validate sample rate
    if (sampleRate > 0.0f) {
        m_sampleRate = sampleRate;
    }
}
