#include "ADSREnvelope.h"
#include <algorithm>

ADSREnvelope::ADSREnvelope(float attackTime, float decayTime, float sustainLevel, float releaseTime)
    : currentStage(Stage::Idle),
      attackTime(std::max(0.001f, attackTime)),        // Minimum 1ms attack
      decayTime(std::max(0.001f, decayTime)),          // Minimum 1ms decay  
      sustainLevel(std::min(std::max(sustainLevel, 0.0f), 1.0f)), // Clamp to valid range
      releaseTime(std::max(0.001f, releaseTime)),      // Minimum 1ms release
      currentLevel(0.0f),
    currentSample(0.0f),
    releaseStartLevel(0.0f)
{
}

float ADSREnvelope::process(bool noteOn, float sampleRate)
{
    // Validate sample rate
    if (sampleRate <= 0.0f) {
        return currentLevel;
    }
    
    float samplesPerSecond = sampleRate;
    
    // Handle note transitions
    if (noteOn && currentStage == Stage::Idle) {
        // Note on: start attack phase
        currentStage = Stage::Attack;
        currentSample = 0.0f;
        releaseStartLevel = 0.0f;
    } else if (!noteOn && (currentStage == Stage::Attack || 
                          currentStage == Stage::Decay || 
                          currentStage == Stage::Sustain)) {
        // Note off: start release phase
        currentStage = Stage::Release;
        currentSample = 0.0f;
        releaseStartLevel = currentLevel;
    }
    
    // Process current stage
    switch (currentStage) {
        case Stage::Attack: {
            float attackSamples = attackTime * samplesPerSecond;
            if (currentSample < attackSamples) {
                // Linear rise to 1.0
                currentLevel = currentSample / attackSamples;
                currentSample += 1.0f;
            } else {
                // Attack complete, move to decay
                currentLevel = 1.0f;
                currentStage = Stage::Decay;
                currentSample = 0.0f;
            }
            break;
        }
        
        case Stage::Decay: {
            float decaySamples = decayTime * samplesPerSecond;
            if (currentSample < decaySamples) {
                // Linear fall from 1.0 to sustainLevel
                float progress = currentSample / decaySamples;
                currentLevel = 1.0f - progress * (1.0f - sustainLevel);
                currentSample += 1.0f;
            } else {
                // Decay complete, move to sustain
                currentLevel = sustainLevel;
                currentStage = Stage::Sustain;
                currentSample = 0.0f;
            }
            break;
        }
        
        case Stage::Sustain:
            // Hold at sustain level
            currentLevel = sustainLevel;
            break;
            
        case Stage::Release: {
            float releaseSamples = releaseTime * samplesPerSecond;
            if (releaseSamples <= 0.0f) {
                currentLevel = 0.0f;
                currentStage = Stage::Idle;
                currentSample = 0.0f;
                break;
            }

            if (currentSample < releaseSamples) {
                // Linear fall to 0.0 from the level captured at release start
                float progress = currentSample / releaseSamples;
                currentLevel = releaseStartLevel * (1.0f - progress);
                currentSample += 1.0f;
            } else {
                // Release complete, go idle
                currentLevel = 0.0f;
                currentStage = Stage::Idle;
                currentSample = 0.0f;
                releaseStartLevel = 0.0f;
            }
            break;
        }
        
        case Stage::Idle:
        default:
            currentLevel = 0.0f;
            break;
    }
    
    return currentLevel;
}

void ADSREnvelope::reset()
{
    currentStage = Stage::Idle;
    currentLevel = 0.0f;
    currentSample = 0.0f;
    releaseStartLevel = 0.0f;
}