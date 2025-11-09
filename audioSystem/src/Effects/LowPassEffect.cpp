#include "LowPassEffect.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float kMixMin = 0.0f;
constexpr float kMixMax = 1.0f;
}

LowPassEffect::LowPassEffect(float cutoff, float sampleRate, float resonance, float mix)
    : m_cutoff(kMinCutoff)
    , m_sampleRate(std::max(sampleRate, kMinSampleRate))
    , m_q(clampValue(resonance, kMinQ, kMaxQ))
    , m_mix(clampValue(mix, kMixMin, kMixMax))
    , m_b0(0.0f)
    , m_b1(0.0f)
    , m_b2(0.0f)
    , m_a1(0.0f)
    , m_a2(0.0f)
    , m_leftState{0.0f, 0.0f}
    , m_rightState{0.0f, 0.0f}
{
    setCutoff(cutoff);
}

std::pair<float, float> LowPassEffect::process(std::pair<float, float> stereoSample)
{
    const float dryLeft = stereoSample.first;
    float wetLeft = m_b0 * dryLeft + m_leftState.z1;
    m_leftState.z1 = m_b1 * dryLeft + m_leftState.z2 - m_a1 * wetLeft;
    m_leftState.z2 = m_b2 * dryLeft - m_a2 * wetLeft;
    const float outLeft = (1.0f - m_mix) * dryLeft + m_mix * wetLeft;

    const float dryRight = stereoSample.second;
    float wetRight = m_b0 * dryRight + m_rightState.z1;
    m_rightState.z1 = m_b1 * dryRight + m_rightState.z2 - m_a1 * wetRight;
    m_rightState.z2 = m_b2 * dryRight - m_a2 * wetRight;
    const float outRight = (1.0f - m_mix) * dryRight + m_mix * wetRight;

    return {outLeft, outRight};
}

void LowPassEffect::reset()
{
    m_leftState = {0.0f, 0.0f};
    m_rightState = {0.0f, 0.0f};
}

void LowPassEffect::setSampleRate(float sampleRate)
{
    const float clampedRate = std::max(sampleRate, kMinSampleRate);
    if (std::fabs(clampedRate - m_sampleRate) < 1e-3f)
    {
        return;
    }

    m_sampleRate = clampedRate;
    m_cutoff = clampValue(m_cutoff, kMinCutoff, (m_sampleRate * 0.5f) * kMaxCutoffRatio);
    updateCoefficients();
}

void LowPassEffect::setCutoff(float cutoff)
{
    const float maxCutoff = (m_sampleRate * 0.5f) * kMaxCutoffRatio;
    const float clamped = clampValue(cutoff, kMinCutoff, std::max(maxCutoff, kMinCutoff));
    if (std::fabs(clamped - m_cutoff) < 1e-3f)
    {
        return;
    }

    m_cutoff = clamped;
    updateCoefficients();
}

void LowPassEffect::setResonance(float resonance)
{
    const float clamped = clampValue(resonance, kMinQ, kMaxQ);
    if (std::fabs(clamped - m_q) < 1e-3f)
    {
        return;
    }

    m_q = clamped;
    updateCoefficients();
}

void LowPassEffect::setMix(float mix)
{
    m_mix = clampValue(mix, kMixMin, kMixMax);
}

float LowPassEffect::clampValue(float value, float low, float high)
{
    return (value < low) ? low : (value > high ? high : value);
}

void LowPassEffect::updateCoefficients()
{
    const float nyquist = m_sampleRate * 0.5f;
    if (nyquist <= kMinCutoff)
    {
        m_b0 = 1.0f;
        m_b1 = 0.0f;
        m_b2 = 0.0f;
        m_a1 = 0.0f;
        m_a2 = 0.0f;
        return;
    }

    const float maxCutoff = nyquist * kMaxCutoffRatio;
    const float cutoff = clampValue(m_cutoff, kMinCutoff, std::max(maxCutoff, kMinCutoff));
    const float omega = 2.0f * static_cast<float>(M_PI) * cutoff / m_sampleRate;
    const float sinw = std::sin(omega);
    const float cosw = std::cos(omega);
    const float alpha = sinw / (2.0f * m_q);

    const float b0 = (1.0f - cosw) * 0.5f;
    const float b1 = 1.0f - cosw;
    const float b2 = (1.0f - cosw) * 0.5f;
    const float a0 = 1.0f + alpha;
    const float a1 = -2.0f * cosw;
    const float a2 = 1.0f - alpha;

    const float invA0 = 1.0f / a0;
    m_b0 = b0 * invA0;
    m_b1 = b1 * invA0;
    m_b2 = b2 * invA0;
    m_a1 = a1 * invA0;
    m_a2 = a2 * invA0;
}
