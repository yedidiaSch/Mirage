#include "audioDevice.h"
#include "RtAudio.h"

AudioDevice::AudioDevice(AudioSystem* audioSystem, float sampleRate, unsigned int bufferFrames) :
                                                                    itsAudioSystem  (audioSystem),
                                                                    m_dac          (std::make_unique<RtAudio>()),
                                                                    m_sampleRate    (sampleRate),
                                                                    m_bufferFrames  (bufferFrames) 
{
    if (m_dac->getDeviceCount() < 1) 
    {
        std::cerr << "No audio devices found!" << std::endl;
        exit(EXIT_FAILURE);
    }

    m_dac->showWarnings(true);

    RtAudio::StreamParameters parameters;
    parameters.deviceId = m_dac->getDefaultOutputDevice();
    parameters.nChannels = 2;  // Stereo output (2 channels)
    parameters.firstChannel = 0;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_SCHEDULE_REALTIME | RTAUDIO_MINIMIZE_LATENCY;
#ifdef RTAUDIO_HOG_DEVICE
    options.flags |= RTAUDIO_HOG_DEVICE;
#endif
    options.numberOfBuffers = 2;
    options.priority = 60; // Request elevated realtime scheduling if available

    unsigned int frames = m_bufferFrames;

    try {
        m_dac->openStream(&parameters, nullptr, RTAUDIO_FLOAT32, 
                         static_cast<unsigned int>(sampleRate), &frames, 
                         &AudioDevice::audioCallback, this, &options);
        m_bufferFrames = frames;
        const double bufferMs = (static_cast<double>(m_bufferFrames) / m_sampleRate) * 1000.0;
        std::cout << "Audio buffer configured: " << m_bufferFrames << " frames (~" << bufferMs << " ms)" << std::endl;
    } catch (RtAudioError& error) {
        std::cerr << "Failed to open audio stream: " << error.getMessage() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void AudioDevice::start() 
{
    try {
        m_dac->startStream();
    } catch (RtAudioError& error) {
        std::cerr << "Failed to start audio stream: " << error.getMessage() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void AudioDevice::stop() 
{
    if (m_dac->isStreamRunning()) m_dac->stopStream();
}

AudioDevice::~AudioDevice() 
{
    if (m_dac->isStreamOpen()) m_dac->closeStream();
}

int AudioDevice::audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
                                double /*streamTime*/, RtAudioStreamStatus /*status*/, void* userData) 
{
    auto* device = static_cast<AudioDevice*>(userData);
    float* buffer = static_cast<float*>(outputBuffer);

    for (unsigned int i = 0; i < nBufferFrames; i++) 
    {
        // Get the next stereo sample from the AudioSystem
        std::pair<float, float> stereoSample = device->itsAudioSystem->getNextSample();
    
        // Write the processed stereo sample to the buffer
        buffer[2 * i] = stereoSample.first;  // Left channel
        buffer[2 * i + 1] = stereoSample.second;  // Right channel
    }

    return 0;
}
