#include "DelayEffect.h"

#include <algorithm>
#include <cmath>

namespace
{
template <typename T>
inline T clampValue(T value, T low, T high)
{
    return (value < low) ? low : (value > high ? high : value);
}
}

// -----------------------------------------------------------------------------
// DelayEffect implementation
// -----------------------------------------------------------------------------

DelayEffect::DelayEffect(float delayTime, float feedback, float mix, float sampleRate)
    : m_bufferLeft{}
    , m_bufferRight{}
    , m_writeIndex(0)
    , m_delaySamples(1)
    , m_delayTime(clampValue(delayTime, kMinDelaySeconds, kMaxDelaySeconds))
    , m_feedback(clampValue(feedback, 0.0f, kMaxFeedback))
    , m_mix(clampValue(mix, 0.0f, 1.0f))
    , m_sampleRate(std::max(sampleRate, 100.0f))
{
    allocateBuffers();
    updateDelaySamples();
    reset();
}

std::pair<float, float> DelayEffect::process(std::pair<float, float> stereoSample)
{
    const std::size_t length = bufferLength();
    if (length == 0U)
    {
        return stereoSample;
    }

    const std::size_t readIndex = (m_writeIndex + length - m_delaySamples) % length;
    const float delayedLeft = m_bufferLeft[readIndex];
    const float delayedRight = m_bufferRight[readIndex];

    const float feedbackLeft = stereoSample.first + delayedLeft * m_feedback;
    const float feedbackRight = stereoSample.second + delayedRight * m_feedback;

    m_bufferLeft[m_writeIndex] = clampValue(feedbackLeft, -2.0f, 2.0f);
    m_bufferRight[m_writeIndex] = clampValue(feedbackRight, -2.0f, 2.0f);

    const float dryCoeff = 1.0f - m_mix;
    const float wetCoeff = m_mix;

    const float outLeft = dryCoeff * stereoSample.first + wetCoeff * delayedLeft;
    const float outRight = dryCoeff * stereoSample.second + wetCoeff * delayedRight;

    m_writeIndex = (m_writeIndex + 1U) % length;

    return {outLeft, outRight};
}

void DelayEffect::reset()
{
    std::fill(m_bufferLeft.begin(), m_bufferLeft.end(), 0.0f);
    std::fill(m_bufferRight.begin(), m_bufferRight.end(), 0.0f);
    m_writeIndex = 0U;
}

void DelayEffect::setSampleRate(float sampleRate)
{
    if (sampleRate <= 100.0f)
    {
        return; // Ignore unreasonable values
    }

    if (std::abs(sampleRate - m_sampleRate) < 1e-3f)
    {
        return; // No meaningful change
    }

    m_sampleRate = sampleRate;
    allocateBuffers();
    updateDelaySamples();
}

void DelayEffect::setDelayTime(float delayTime)
{
    const float clamped = clampValue(delayTime, kMinDelaySeconds, kMaxDelaySeconds);
    if (std::abs(clamped - m_delayTime) < 1e-6f)
    {
        return;
    }

    m_delayTime = clamped;
    updateDelaySamples();
}

void DelayEffect::setFeedback(float feedback)
{
    m_feedback = clampValue(feedback, 0.0f, kMaxFeedback);
}

void DelayEffect::setMix(float mix)
{
    m_mix = clampValue(mix, 0.0f, 1.0f);
}

void DelayEffect::allocateBuffers()
{
    const std::size_t requiredSamples = static_cast<std::size_t>(std::ceil(kMaxDelaySeconds * m_sampleRate)) + 1U;
    const std::size_t targetSize = std::max<std::size_t>(requiredSamples, 2U);

    if (targetSize != bufferLength())
    {
        m_bufferLeft.assign(targetSize, 0.0f);
        m_bufferRight.assign(targetSize, 0.0f);
    }

    if (m_writeIndex >= targetSize)
    {
        m_writeIndex = 0U;
    }

    if (m_delaySamples >= targetSize)
    {
        m_delaySamples = targetSize - 1U;
    }
}

void DelayEffect::updateDelaySamples()
{
    const std::size_t length = bufferLength();
    if (length == 0U)
    {
        return;
    }

    const std::size_t samples = static_cast<std::size_t>(std::round(m_delayTime * m_sampleRate));
    m_delaySamples = clampValue<std::size_t>(samples, 1U, length - 1U);
}
