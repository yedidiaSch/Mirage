#pragma once

#include <napi.h>
#include <memory>
#include <vector>
#include <limits>
#include "../../audioSystem/src/Core/audioSystem.h"
#include "../../audioSystem/src/Core/audioDevice.h"
#include "../../audioSystem/src/Midi/MidiDevice.h"
#include "../../audioSystem/src/Adapters/AudioSystemAdapter.h"

class StereoSampleRingBuffer;

/**
 * @class AudioSystemWrapper
 * @brief N-API wrapper for the C++ AudioSystem and AudioDevice classes
 * 
 * This class exposes the C++ AudioSystem functionality to JavaScript/Node.js
 * using N-API. It includes AudioDevice to handle actual sound output.
 */
class AudioSystemWrapper : public Napi::ObjectWrap<AudioSystemWrapper>
{
public:
    /**
     * @brief Initialize the N-API wrapper class
     * @param env N-API environment
     * @param exports Module exports object
     * @return Exports object with the AudioSystem class attached
     */
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    /**
     * @brief Constructor
     * @param info Callback info containing the sample rate and buffer size
     */
    AudioSystemWrapper(const Napi::CallbackInfo& info);

    /**
     * @brief Destructor - cleanup audio device
     */
    ~AudioSystemWrapper();

private:
    std::unique_ptr<StereoSampleRingBuffer> m_waveformBuffer;
    std::unique_ptr<AudioSystem> m_audioSystem;
    std::unique_ptr<AudioDevice> m_audioDevice;
    std::unique_ptr<MidiDevice> m_midiDevice;
    std::unique_ptr<AudioSystemAdapter> m_adapter;
    std::string m_midiDeviceName;
    float m_sampleRate;
    float m_currentFrequency;
    std::vector<float> m_activeFrequencies;

    // JavaScript-accessible methods
    Napi::Value Start(const Napi::CallbackInfo& info);
    Napi::Value Stop(const Napi::CallbackInfo& info);
    Napi::Value TriggerNote(const Napi::CallbackInfo& info);
    Napi::Value TriggerNoteOff(const Napi::CallbackInfo& info);
    Napi::Value ResetEffects(const Napi::CallbackInfo& info);
    Napi::Value ClearEffects(const Napi::CallbackInfo& info);
    Napi::Value UpdateADSRParameters(const Napi::CallbackInfo& info);
    Napi::Value SetWaveform(const Napi::CallbackInfo& info);
    Napi::Value AddDelayEffect(const Napi::CallbackInfo& info);
    Napi::Value AddLowPassEffect(const Napi::CallbackInfo& info);
    Napi::Value SetLowPassCutoff(const Napi::CallbackInfo& info);
    Napi::Value GetLowPassCutoff(const Napi::CallbackInfo& info);
    Napi::Value AddOctaveEffect(const Napi::CallbackInfo& info);
    Napi::Value SetDriftParameters(const Napi::CallbackInfo& info);
    Napi::Value GetMidiStatus(const Napi::CallbackInfo& info);
    Napi::Value GetRecentWaveform(const Napi::CallbackInfo& info);
    Napi::Value ConfigureSecondaryOscillator(const Napi::CallbackInfo& info);
    Napi::Value SetPitchBend(const Napi::CallbackInfo& info);
};
