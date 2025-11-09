#pragma once

#include "IEffect.h"

#include <utility>

/**
 * @brief Resonant low-pass biquad filter
 *
 * Implements a 2nd-order (12 dB/oct) low-pass filter with adjustable
 * cutoff frequency, resonance (Q), and dry/wet mix. The effect uses the
 * RBJ cookbook formula for coefficient calculation and processes samples
 * in Direct Form II Transposed for numerical stability.
 */
class LowPassEffect : public IEffect
{
public:
    LowPassEffect(float cutoff = 1200.0f,
                  float sampleRate = 44100.0f,
                  float resonance = 0.9f,
                  float mix = 1.0f);

    std::pair<float, float> process(std::pair<float, float> stereoSample) override;
    void reset() override;

    void setSampleRate(float sampleRate);
    void setCutoff(float cutoff);
    void setResonance(float resonance);
    void setMix(float mix);

    float getCutoff() const { return m_cutoff; }

private:
    static constexpr float kMinCutoff = 20.0f;
    static constexpr float kMaxCutoffRatio = 0.45f; // relative to Nyquist
    static constexpr float kMinSampleRate = 100.0f;
    static constexpr float kMinQ = 0.1f;
    static constexpr float kMaxQ = 10.0f;

    float m_cutoff;
    float m_sampleRate;
    float m_q;
    float m_mix;

    float m_b0;
    float m_b1;
    float m_b2;
    float m_a1;
    float m_a2;

    struct FilterState
    {
        float z1;
        float z2;
    };

    FilterState m_leftState;
    FilterState m_rightState;

    static float clampValue(float value, float low, float high);
    void updateCoefficients();
};
