#pragma once

#include <atomic>
#include <cstddef>
#include <vector>
#include <algorithm>

/**
 * @brief Lock-free ring buffer to capture recent stereo samples for visualization.
 *
 * Designed for a single producer (audio callback thread) and a single consumer
 * (UI thread). The producer writes interleaved stereo frames via push(), while the
 * consumer copies the most recent frames into its own buffer without blocking.
 */
class StereoSampleRingBuffer {
public:
    explicit StereoSampleRingBuffer(std::size_t capacityFrames)
        : m_capacityFrames(std::max<std::size_t>(1, capacityFrames)),
          m_buffer(m_capacityFrames * 2, 0.0f),
          m_writeIndex(0),
          m_totalFramesWritten(0) {}

    /**
     * @return Maximum number of frames stored in the ring buffer.
     */
    std::size_t capacityFrames() const noexcept { return m_capacityFrames; }

    /**
     * @brief Append a stereo frame to the ring buffer.
     *
     * Uses relaxed operations for minimal overhead and releases the new write
     * position so readers observe freshly written samples.
     */
    void push(float left, float right) noexcept {
        std::size_t frameIndex = m_writeIndex.load(std::memory_order_relaxed);
        const std::size_t sampleIndex = frameIndex * 2U;
        m_buffer[sampleIndex] = left;
        m_buffer[sampleIndex + 1U] = right;

        frameIndex += 1U;
        if (frameIndex >= m_capacityFrames) {
            frameIndex = 0U;
        }

        m_writeIndex.store(frameIndex, std::memory_order_release);
        m_totalFramesWritten.fetch_add(1U, std::memory_order_release);
    }

    /**
     * @return Number of frames currently available to copy.
     */
    std::size_t availableFrames() const noexcept {
        const std::size_t written = m_totalFramesWritten.load(std::memory_order_acquire);
        return std::min(written, m_capacityFrames);
    }

    /**
     * @brief Copy the most recent frames into an interleaved output buffer.
     *
     * @param dest Pointer to a buffer with space for maxFrames * 2 floats.
     * @param maxFrames Maximum number of frames to copy.
     * @return Actual number of frames copied.
     */
    std::size_t copyLatestInterleaved(float* dest, std::size_t maxFrames) const noexcept {
        if (!dest || maxFrames == 0U) {
            return 0U;
        }

        const std::size_t available = availableFrames();
        const std::size_t framesToCopy = std::min(maxFrames, available);
        if (framesToCopy == 0U) {
            return 0U;
        }

        const std::size_t writeIndex = m_writeIndex.load(std::memory_order_acquire);
        std::size_t startFrame = writeIndex + m_capacityFrames - framesToCopy;
        if (startFrame >= m_capacityFrames) {
            startFrame %= m_capacityFrames;
        }

        for (std::size_t i = 0U; i < framesToCopy; ++i) {
            std::size_t frameIndex = startFrame + i;
            if (frameIndex >= m_capacityFrames) {
                frameIndex -= m_capacityFrames;
            }
            const std::size_t sampleIndex = frameIndex * 2U;
            dest[(i * 2U)] = m_buffer[sampleIndex];
            dest[(i * 2U) + 1U] = m_buffer[sampleIndex + 1U];
        }

        return framesToCopy;
    }

private:
    const std::size_t m_capacityFrames;
    std::vector<float> m_buffer;
    mutable std::atomic<std::size_t> m_writeIndex;
    mutable std::atomic<std::size_t> m_totalFramesWritten;
};
