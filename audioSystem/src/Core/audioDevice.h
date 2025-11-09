#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "audioSystem.h"
#include "Effects/IEffect.h"
#include "RtAudio.h"

/**
 * @class AudioDevice
 * @brief Manages audio input/output hardware interactions using RtAudio
 *
 * This class provides an interface to the physical audio hardware through RtAudio.
 * It handles setting up audio streams, managing buffer sizes, and coordinating
 * real-time audio processing through the AudioSystem.
 */
class AudioDevice 
{

public:

    /**
     * @brief Constructor for AudioDevice
     * @param audioSystem Pointer to the AudioSystem that will process audio data
     * @param sampleRate The sample rate to use for audio processing (e.g., 44100, 48000)
     * @param bufferFrames The number of frames per audio buffer
     */
    AudioDevice                 (AudioSystem* audioSystem, float sampleRate, unsigned int bufferFrames);

    /**
     * @brief Destructor - ensures proper cleanup of audio resources
     */
    ~AudioDevice                ();

    /**
     * @brief Starts the audio processing stream
     * 
     * Opens the audio stream and begins real-time audio processing using the
     * configured parameters and callback function.
     */
    void start                  ();

    /**
     * @brief Stops the audio processing stream
     * 
     * Safely closes the audio stream and stops all processing.
     */
    void stop                   ();

private:

    /**
     * @brief Static callback function called by RtAudio when audio data is needed
     * @param outputBuffer Pointer to the output (playback) buffer
     * @param inputBuffer Pointer to the input (recording) buffer
     * @param nBufferFrames Number of frames in the current buffer
     * @param streamTime The stream time in seconds
     * @param status Status flag indicating potential stream problems
     * @param userData User data pointer (typically points to the AudioDevice instance)
     * @return 0 on success, non-zero on error
     * 
     * This callback is invoked by RtAudio whenever new audio data is needed.
     * It routes processing to the AudioSystem for audio generation and effects.
     */
    static int audioCallback    (void* outputBuffer,
                                 void* inputBuffer,
                                 unsigned int nBufferFrames,
                                 double streamTime,
                                 RtAudioStreamStatus status,
                                 void* userData);

    /**
     * @brief Pointer to the AudioSystem that processes audio data
     * 
     * The AudioSystem handles synthesis, effects processing, and other
     * audio operations that occur within the audio callback.
     */
    AudioSystem*        itsAudioSystem;

    /**
     * @brief RtAudio instance that interfaces with audio hardware
     */
    std::unique_ptr<RtAudio> m_dac;

    /**
     * @brief The sample rate used for audio processing (in Hz)
     */
    float               m_sampleRate;
    
    /**
     * @brief The number of frames per audio buffer
     * 
     * Controls latency - smaller values reduce latency but increase CPU usage
     * and risk of audio dropouts.
     */
    unsigned int        m_bufferFrames;
};
