#include "AudioSystemWrapper.h"
#include "../../audioSystem/src/Core/StereoSampleRingBuffer.h"
#include "../../audioSystem/src/Waves/SineWave.h"
#include "../../audioSystem/src/Waves/SquareWave.h"
#include "../../audioSystem/src/Waves/SawtoothWave.h"
#include "../../audioSystem/src/Waves/TriangleWave.h"
#include "../../audioSystem/src/Effects/DelayEffect.h"
#include "../../audioSystem/src/Effects/LowPassEffect.h"
#include "../../audioSystem/src/Effects/OctaveEffect.h"
#include <algorithm>
#include <cmath>
#include <iterator>

Napi::Object AudioSystemWrapper::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env, "AudioSystem", {
        InstanceMethod("start", &AudioSystemWrapper::Start),
        InstanceMethod("stop", &AudioSystemWrapper::Stop),
        InstanceMethod("triggerNote", &AudioSystemWrapper::TriggerNote),
        InstanceMethod("triggerNoteOff", &AudioSystemWrapper::TriggerNoteOff),
        InstanceMethod("resetEffects", &AudioSystemWrapper::ResetEffects),
        InstanceMethod("clearEffects", &AudioSystemWrapper::ClearEffects),
        InstanceMethod("updateADSRParameters", &AudioSystemWrapper::UpdateADSRParameters),
        InstanceMethod("setWaveform", &AudioSystemWrapper::SetWaveform),
        InstanceMethod("addDelayEffect", &AudioSystemWrapper::AddDelayEffect),
        InstanceMethod("addLowPassEffect", &AudioSystemWrapper::AddLowPassEffect),
        InstanceMethod("setLowPassCutoff", &AudioSystemWrapper::SetLowPassCutoff),
        InstanceMethod("getLowPassCutoff", &AudioSystemWrapper::GetLowPassCutoff),
        InstanceMethod("addOctaveEffect", &AudioSystemWrapper::AddOctaveEffect),
    InstanceMethod("setDriftParameters", &AudioSystemWrapper::SetDriftParameters),
    InstanceMethod("getMidiStatus", &AudioSystemWrapper::GetMidiStatus),
    InstanceMethod("getRecentWaveform", &AudioSystemWrapper::GetRecentWaveform),
    InstanceMethod("configureSecondaryOscillator", &AudioSystemWrapper::ConfigureSecondaryOscillator),
    InstanceMethod("setPitchBend", &AudioSystemWrapper::SetPitchBend)
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("AudioSystem", func);
    return exports;
}

AudioSystemWrapper::AudioSystemWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<AudioSystemWrapper>(info),
      m_sampleRate(44100.0f),
      m_currentFrequency(0.0f)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Number expected for sample rate")
            .ThrowAsJavaScriptException();
        return;
    }

    m_sampleRate = info[0].As<Napi::Number>().FloatValue();
    unsigned int bufferFrames = 512; // Lower default buffer size for reduced latency
    
    if (info.Length() >= 2 && info[1].IsNumber())
    {
        bufferFrames = info[1].As<Napi::Number>().Uint32Value();
    }
    m_waveformBuffer = std::make_unique<StereoSampleRingBuffer>(
        static_cast<std::size_t>(std::max(2048.0f, m_sampleRate * 0.5f)));
    m_audioSystem = std::make_unique<AudioSystem>(m_sampleRate);
    m_audioSystem->setWaveformTapBuffer(m_waveformBuffer.get());
    m_audioDevice = std::make_unique<AudioDevice>(m_audioSystem.get(), m_sampleRate, bufferFrames);
    
    // Initialize MIDI device and adapter
    try {
        // Create a temporary MidiDevice to list ports
        RtMidiIn tempMidi;
        unsigned int nPorts = tempMidi.getPortCount();
        
        int selectedPort = 0; // Default to first port
        bool foundHardware = false;
        
        std::cout << "Scanning for MIDI devices..." << std::endl;
        for (unsigned int i = 0; i < nPorts; i++) {
            std::string portName = tempMidi.getPortName(i);
            std::cout << "  Port " << i << ": " << portName << std::endl;
            
            // Skip "Midi Through", "Announce", "Timer", and PipeWire virtual ports
            if (!foundHardware && 
                portName.find("Midi Through") == std::string::npos &&
                portName.find("Announce") == std::string::npos &&
                portName.find("Timer") == std::string::npos &&
                portName.find("PipeWire") == std::string::npos) {
                selectedPort = i;
                foundHardware = true;
                std::cout << "  -> Selected hardware MIDI controller: " << portName << std::endl;
            }
        }
        
        if (nPorts > 0) {
            m_midiDevice = std::make_unique<MidiDevice>(selectedPort);
            m_midiDeviceName = tempMidi.getPortName(selectedPort);
            m_adapter = std::make_unique<AudioSystemAdapter>(m_audioSystem.get());
            m_midiDevice->attach(m_adapter.get());
            m_midiDevice->start();
            std::cout << "MIDI device ready!" << std::endl;
        } else {
            m_midiDeviceName = "";
            std::cout << "No MIDI ports found - keyboard/mouse input will still work" << std::endl;
        }
    } catch (const std::exception& e) {
        m_midiDeviceName = "";
        std::cerr << "Warning: Failed to initialize MIDI: " << e.what() << std::endl;
        std::cerr << "Continuing without MIDI support (keyboard/mouse input will still work)" << std::endl;
    }
}

AudioSystemWrapper::~AudioSystemWrapper()
{
    if (m_audioSystem)
    {
        m_audioSystem->setWaveformTapBuffer(nullptr);
    }
    if (m_midiDevice)
    {
        m_midiDevice->stop();
    }
    if (m_audioDevice)
    {
        m_audioDevice->stop();
    }
}

Napi::Value AudioSystemWrapper::Start(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    
    try
    {
        m_audioDevice->start();
    }
    catch (const std::exception& e)
    {
        Napi::Error::New(env, std::string("Failed to start audio: ") + e.what())
            .ThrowAsJavaScriptException();
    }
    
    return env.Undefined();
}

Napi::Value AudioSystemWrapper::Stop(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    m_audioDevice->stop();
    m_activeFrequencies.clear();
    m_currentFrequency = 0.0f;
    return env.Undefined();
}

Napi::Value AudioSystemWrapper::TriggerNote(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Number expected for frequency")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    m_currentFrequency = info[0].As<Napi::Number>().FloatValue();
    m_audioSystem->triggerNote(m_currentFrequency);
    m_activeFrequencies.push_back(m_currentFrequency);

    return env.Undefined();
}

Napi::Value AudioSystemWrapper::TriggerNoteOff(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    float frequency = std::numeric_limits<float>::quiet_NaN();

    if (info.Length() >= 1 && info[0].IsNumber())
    {
        frequency = info[0].As<Napi::Number>().FloatValue();
    }
    else if (!m_activeFrequencies.empty())
    {
        frequency = m_activeFrequencies.back();
    }

    if (!std::isnan(frequency))
    {
        auto matcher = [frequency](float active)
        {
            return std::fabs(active - frequency) < 1e-3f;
        };
        auto it = std::find_if(m_activeFrequencies.rbegin(), m_activeFrequencies.rend(), matcher);
        if (it != m_activeFrequencies.rend())
        {
            m_activeFrequencies.erase(std::next(it).base());
        }
    }
    else
    {
        m_activeFrequencies.clear();
    }

    m_audioSystem->triggerNoteOff(frequency);
    if (m_activeFrequencies.empty())
    {
        m_currentFrequency = 0.0f;
    }
    return env.Undefined();
}

Napi::Value AudioSystemWrapper::ResetEffects(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    m_audioSystem->resetEffects();
    return env.Undefined();
}

Napi::Value AudioSystemWrapper::ClearEffects(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    m_audioSystem->clearEffects();
    return env.Undefined();
}

Napi::Value AudioSystemWrapper::UpdateADSRParameters(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 4 || !info[0].IsNumber() || !info[1].IsNumber() || 
        !info[2].IsNumber() || !info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Four numbers expected (attack, decay, sustain, release)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    float attack = info[0].As<Napi::Number>().FloatValue();
    float decay = info[1].As<Napi::Number>().FloatValue();
    float sustain = info[2].As<Napi::Number>().FloatValue();
    float release = info[3].As<Napi::Number>().FloatValue();

    m_audioSystem->updateADSRParameters(attack, decay, sustain, release);

    return env.Undefined();
}

Napi::Value AudioSystemWrapper::SetWaveform(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected for waveform type")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string waveformType = info[0].As<Napi::String>().Utf8Value();
    std::shared_ptr<IWave> waveform;

    if (waveformType == "sine")
    {
        waveform = std::make_shared<SineWave>();
    }
    else if (waveformType == "square")
    {
        waveform = std::make_shared<SquareWave>();
    }
    else if (waveformType == "saw")
    {
        waveform = std::make_shared<SawtoothWave>();
    }
    else if (waveformType == "triangle")
    {
        waveform = std::make_shared<TriangleWave>();
    }
    else
    {
        Napi::TypeError::New(env, "Unknown waveform type. Use: sine, square, saw, or triangle")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    m_audioSystem->setWaveform(waveform);
    return env.Undefined();
}

Napi::Value AudioSystemWrapper::AddDelayEffect(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 3 || !info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Three numbers expected (delayTime, feedback, mix)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    float delayTime = info[0].As<Napi::Number>().FloatValue();
    float feedback = info[1].As<Napi::Number>().FloatValue();
    float mix = info[2].As<Napi::Number>().FloatValue();
    
    auto effect = std::make_shared<DelayEffect>(delayTime, feedback, mix, m_sampleRate);
    m_audioSystem->addEffect(effect);

    return env.Undefined();
}

Napi::Value AudioSystemWrapper::AddLowPassEffect(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Number expected for cutoff frequency")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    float cutoff = info[0].As<Napi::Number>().FloatValue();

    float resonance = 0.9f;
    float mix = 1.0f;

    if (info.Length() >= 2)
    {
        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "Resonance must be a number")
                .ThrowAsJavaScriptException();
            return env.Null();
        }
        resonance = info[1].As<Napi::Number>().FloatValue();
    }

    if (info.Length() >= 3)
    {
        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "Mix must be a number")
                .ThrowAsJavaScriptException();
            return env.Null();
        }
        mix = info[2].As<Napi::Number>().FloatValue();
    }
    
    auto effect = std::make_shared<LowPassEffect>(cutoff, m_sampleRate, resonance, mix);
    m_audioSystem->addEffect(effect);

    return env.Undefined();
}

Napi::Value AudioSystemWrapper::SetLowPassCutoff(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Number expected for cutoff frequency")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    const float cutoff = info[0].As<Napi::Number>().FloatValue();
    m_audioSystem->setLowPassCutoff(cutoff);

    return env.Undefined();
}

Napi::Value AudioSystemWrapper::GetLowPassCutoff(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    const float cutoff = m_audioSystem->getLowPassCutoff();
    return Napi::Number::New(env, cutoff);
}

Napi::Value AudioSystemWrapper::AddOctaveEffect(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsBoolean() || !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Boolean and Number expected (higher, blend)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    bool higher = info[0].As<Napi::Boolean>().Value();
    float blend = info[1].As<Napi::Number>().FloatValue();
    
    auto effect = std::make_shared<OctaveEffect>(higher, blend);
    
    // Initialize the octave effect with current frequency and sample rate
    effect->setSampleRate(m_sampleRate);
    if (m_currentFrequency > 0.0f) {
        effect->setFrequency(m_currentFrequency);
    }
    
    m_audioSystem->addEffect(effect);

    return env.Undefined();
}

Napi::Value AudioSystemWrapper::SetDriftParameters(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 3 || !info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Three numbers expected (rateHz, amountCents, jitterCents)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    const float rateHz = info[0].As<Napi::Number>().FloatValue();
    const float amountCents = info[1].As<Napi::Number>().FloatValue();
    const float jitterCents = info[2].As<Napi::Number>().FloatValue();

    m_audioSystem->setDriftParameters(rateHz, amountCents, jitterCents);

    return env.Undefined();
}

Napi::Value AudioSystemWrapper::GetMidiStatus(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::Object result = Napi::Object::New(env);
    
    result.Set("connected", Napi::Boolean::New(env, m_midiDevice != nullptr));
    result.Set("deviceName", Napi::String::New(env, m_midiDeviceName));
    
    return result;
}

Napi::Value AudioSystemWrapper::GetRecentWaveform(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (!m_waveformBuffer)
    {
        return Napi::Float32Array::New(env, 0);
    }

    std::size_t requestedFrames = 1024U;
    if (info.Length() >= 1)
    {
        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "Number expected for maxFrames")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        const double value = info[0].As<Napi::Number>().DoubleValue();
        if (value <= 0.0)
        {
            requestedFrames = 0U;
        }
        else
        {
            requestedFrames = static_cast<std::size_t>(value);
        }
    }

    const std::size_t capacity = m_waveformBuffer->capacityFrames();
    const std::size_t available = m_waveformBuffer->availableFrames();
    std::size_t framesToCopy = std::min(requestedFrames, capacity);
    framesToCopy = std::min(framesToCopy, available);

    Napi::Float32Array result = Napi::Float32Array::New(env, framesToCopy * 2U);
    if (framesToCopy == 0U)
    {
        return result;
    }

    m_waveformBuffer->copyLatestInterleaved(result.Data(), framesToCopy);
    return result;
}

Napi::Value AudioSystemWrapper::ConfigureSecondaryOscillator(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 4 || !info[0].IsBoolean() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Expected arguments: enabled:boolean, mix:number, detuneCents:number, octaveOffset:number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    const bool enabled = info[0].As<Napi::Boolean>().Value();
    const float mix = info[1].As<Napi::Number>().FloatValue();
    const float detune = info[2].As<Napi::Number>().FloatValue();
    const int octaveOffset = info[3].As<Napi::Number>().Int32Value();

    m_audioSystem->configureSecondaryOscillator(enabled, mix, detune, octaveOffset);

    return env.Undefined();
}

Napi::Value AudioSystemWrapper::SetPitchBend(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Number expected for pitch bend value")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    int value = info[0].As<Napi::Number>().Int32Value();
    value = std::max(-8192, std::min(8191, value));

    m_audioSystem->setPitchBend(value);

    return env.Undefined();
}

// Module initialization
Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    return AudioSystemWrapper::Init(env, exports);
}

NODE_API_MODULE(audioSystemNative, InitAll)
