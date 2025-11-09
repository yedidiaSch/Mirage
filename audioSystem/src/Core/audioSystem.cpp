#include <cmath>
#include <algorithm> // For std::find and std::transform
#include <cctype>    // For std::tolower
#include <iterator>
#include <limits>
#include <random>
#include "audioSystem.h"
#include "StereoSampleRingBuffer.h"
#include "Waves/SquareWave.h" // Include the square wave implementation
#include "Waves/SineWave.h"
#include "Waves/SawtoothWave.h"
#include "Waves/TriangleWave.h"
#include "Effects/OctaveEffect.h"
#include "Effects/DelayEffect.h"
#include "Effects/LowPassEffect.h"
#include "Effects/EffectParameters.h"

namespace {
    /**
     * @brief Convert string to lowercase for case-insensitive comparison
     */
    std::string toLowercase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    inline std::mt19937& randomEngine() {
        static std::mt19937 engine{std::random_device{}()};
        return engine;
    }
}

AudioSystem::AudioSystem(float sampleRate) : m_frequency(0.0f),
                                             m_sampleRate(sampleRate > 0.0f ? sampleRate : 44100.0f),
                                             m_primaryPhase(0.0f),
                                             m_secondaryPhase(0.0f),
                                             m_noteOn(false),
                                             m_lfoPhase(0.0f),
                                             m_lfoRateHz(0.35f),
                                             m_lfoAmountCents(4.0f),
                                             m_noteJitterAmountCents(3.0f),
                                             m_noteDetuneCents(0.0f),
                                             m_waveformTap(nullptr),
                                             m_secondaryEnabled(false),
                                             m_secondaryMix(0.0f),
                                             m_secondaryDetuneCents(0.0f),
                                             m_secondaryOctaveOffset(0),
                                             m_pitchBendCents(0.0f),
                                             m_lowPassActive(false),
                                             m_lastLowPassCutoff(0.0f)
{
    // Validate sample rate
    if (sampleRate <= 0.0f) {
        // Use a reasonable default and potentially log warning
        m_sampleRate = 44100.0f;
    }
    
    // Default to square wave
    m_primaryWaveform = std::make_shared<SquareWave>();
    m_secondaryWaveform = m_primaryWaveform;
    
    // Initialize ADSR envelope with default values
    m_envelope = std::make_unique<ADSREnvelope>(0.1f, 0.2f, 0.7f, 0.3f);
}

void AudioSystem::setWaveform(std::shared_ptr<IWave> waveform)
{
    if (waveform) {
        m_primaryWaveform = waveform;
        m_secondaryWaveform = waveform;
    }
}

// Configure the oscillator and effects based on the provided AudioConfig
void AudioSystem::configure(const AudioConfig& config)
{
    // Select waveform (case-insensitive)
    std::string waveformLower = toLowercase(config.waveform);
    
    if (waveformLower == "sine") {
        m_primaryWaveform = std::make_shared<SineWave>();
    } else if (waveformLower == "sawtooth" || waveformLower == "saw") {
        m_primaryWaveform = std::make_shared<SawtoothWave>();
    } else if (waveformLower == "triangle" || waveformLower == "tri") {
        m_primaryWaveform = std::make_shared<TriangleWave>();
    } else if (waveformLower == "square" || waveformLower.empty()) {
        // Default to square wave for empty or unrecognized waveforms
        m_primaryWaveform = std::make_shared<SquareWave>();
    } else {
        // Fallback to square wave for unrecognized waveforms
        m_primaryWaveform = std::make_shared<SquareWave>();
    }

    m_secondaryWaveform = m_primaryWaveform;

    // Clear existing effects
    m_effects.clear();
    m_lowPassActive = false;
    m_lastLowPassCutoff = 0.0f;

    // Instantiate effects listed in the configuration (case-insensitive)
    for (const auto& name : config.effects)
    {
        std::string effectLower = toLowercase(name);
        
        if (effectLower == "octave") {
            auto eff = std::make_shared<OctaveEffect>();
            m_effects.push_back(eff);
        } else if (effectLower == "delay" || effectLower == "echo") {
            auto eff = std::make_shared<DelayEffect>(0.3f, 0.5f, 0.5f, m_sampleRate);
            m_effects.push_back(eff);
        } else if (effectLower == "lowpass" || effectLower == "lpf" || effectLower == "filter") {
            auto eff = std::make_shared<LowPassEffect>(1000.0f, m_sampleRate);
            m_effects.push_back(eff);
            m_lowPassActive = true;
            m_lastLowPassCutoff = eff->getCutoff();
        }
        // Silently ignore unrecognized effect names
    }
    
    // Update ADSR envelope parameters
    if (m_envelope) {
        m_envelope = std::make_unique<ADSREnvelope>(
            config.attackTime,
            config.decayTime,
            config.sustainLevel,
            config.releaseTime
        );
    }
}

void AudioSystem::triggerNote(float newFrequency)
{
    // Validate frequency range (20 Hz to 20 kHz is typical audio range)
    if (newFrequency <= 0.0f || newFrequency > 20000.0f) {
        return; // Ignore invalid frequencies
    }
    
    bool hadActiveNotes = !m_activeNotes.empty();

    const float detune = std::uniform_real_distribution<float>(-m_noteJitterAmountCents, m_noteJitterAmountCents)(randomEngine());

    m_activeNotes.push_back({newFrequency, detune});
    m_frequency = newFrequency;
    m_noteDetuneCents = detune;
    m_noteOn = true;

    if (!hadActiveNotes)
    {
        m_primaryPhase = 0.0f;
        m_secondaryPhase = 0.0f;
        m_lfoPhase = std::uniform_real_distribution<float>(0.0f, 1.0f)(randomEngine());
        if (m_envelope) {
            m_envelope->reset();
        }
    }

    // Configure any effects that need the note frequency or sample rate
    for (auto& effect : m_effects)
    {
        if (auto octave = std::dynamic_pointer_cast<OctaveEffect>(effect))
        {
            octave->setFrequency(newFrequency);
            octave->setSampleRate(m_sampleRate);
        }
        else if (auto delay = std::dynamic_pointer_cast<DelayEffect>(effect))
        {
            delay->setSampleRate(m_sampleRate);
        }
        else if (auto lp = std::dynamic_pointer_cast<LowPassEffect>(effect))
        {
            lp->setSampleRate(m_sampleRate);
        }
    }
    
    // Don't reset effects here - we want to maintain state across notes
    // (e.g., delay buffer should keep echoing from previous notes)
}

void AudioSystem::triggerNoteOff(float frequency) 
{
    if (std::isnan(frequency))
    {
        m_activeNotes.clear();
        m_noteOn = false;
        return;
    }

    auto isMatch = [frequency](const ActiveNote& active)
    {
        return std::fabs(active.frequency - frequency) < 1e-3f;
    };

    auto revIt = std::find_if(m_activeNotes.rbegin(), m_activeNotes.rend(), isMatch);
    if (revIt != m_activeNotes.rend())
    {
        m_activeNotes.erase(std::next(revIt).base());
    }

    if (m_activeNotes.empty())
    {
        m_noteOn = false;
    }
    else
    {
        const auto& active = m_activeNotes.back();
        m_frequency = active.frequency;
        m_noteDetuneCents = active.detuneCents;
        m_noteOn = true;
    }
}

std::pair<float, float> AudioSystem::getNextSample() 
{
    if (!m_primaryWaveform)
    {
        return {0.0f, 0.0f};
    }
    
    // Get envelope amplitude (this handles all ADSR phases including release)
    float envelopeLevel = 0.0f;
    if (m_envelope) {
        envelopeLevel = m_envelope->process(m_noteOn, m_sampleRate);
    }
    
    float sample = 0.0f;

    if (envelopeLevel > 0.0f)
    {
        float modulatedFrequency = m_frequency;

        if (m_frequency > 0.0f)
        {
            constexpr float twoPi = 6.28318530717958647692f;
            float lfoValue = std::sin(twoPi * m_lfoPhase);
            float totalDetuneCents = m_noteDetuneCents + (lfoValue * m_lfoAmountCents) + m_pitchBendCents;
            float detuneRatio = std::pow(2.0f, totalDetuneCents / 1200.0f);
            modulatedFrequency = m_frequency * detuneRatio;

            m_lfoPhase += (m_lfoRateHz / m_sampleRate);
            if (m_lfoPhase >= 1.0f)
            {
                m_lfoPhase -= std::floor(m_lfoPhase);
            }
        }

        float primarySample = m_primaryWaveform->generate(modulatedFrequency, m_sampleRate, m_primaryPhase);

        float secondarySample = 0.0f;
        if (m_secondaryEnabled && m_secondaryMix > 0.0f && m_secondaryWaveform)
        {
            const float detuneRatio = std::pow(2.0f, std::max(m_secondaryDetuneCents, 0.0f) / 1200.0f);
            const float octaveRatio = std::pow(2.0f, static_cast<float>(m_secondaryOctaveOffset));
            const float secondaryFrequency = modulatedFrequency * detuneRatio * octaveRatio;
            secondarySample = m_secondaryWaveform->generate(secondaryFrequency, m_sampleRate, m_secondaryPhase);
        }

    const float dryAmount = std::max(0.0f, 1.0f - m_secondaryMix);
        sample = (primarySample * dryAmount) + (secondarySample * m_secondaryMix);
        sample *= envelopeLevel;
    }

    // Create a stereo sample (initially identical in both channels)
    std::pair<float, float> stereoSample{sample, sample};

    // Apply effects to the stereo sample
    stereoSample = applyEffects(stereoSample);

    if (m_waveformTap) {
        m_waveformTap->push(stereoSample.first, stereoSample.second);
    }

    return stereoSample;
}

std::pair<float, float> AudioSystem::applyEffects(std::pair<float, float> stereoSample) 
{
    // Apply each effect in the chain to the stereo sample
    for (const auto& effect : m_effects) 
    {
        if (effect) { // Null check for safety
            stereoSample = effect->process(stereoSample);
        }
    }

    return stereoSample;
}

void AudioSystem::addEffect(std::shared_ptr<IEffect> effect) 
{
    if (!effect) {
        return; // Don't add null effects
    }
    
    // Check if the effect already exists in the vector
    auto it = std::find(m_effects.begin(), m_effects.end(), effect);
    
    if (it == m_effects.end()) 
    {
        // Effect not found, so add it to the vector
        m_effects.push_back(effect);
        if (auto lowPass = std::dynamic_pointer_cast<LowPassEffect>(effect))
        {
            m_lowPassActive = true;
            m_lastLowPassCutoff = lowPass->getCutoff();
        }
    } 
}


void AudioSystem::resetEffects() 
{
    // Reset internal state of all effects (clear buffers, reset phase, etc.)
    for (const auto& effect : m_effects) 
    {
        if (effect) { // Null check for safety
            effect->reset();
        }
    }
}

void AudioSystem::clearEffects()
{
    // First reset all effects
    resetEffects();
    
    // Then clear the effects vector completely
    m_effects.clear();
    m_lowPassActive = false;
    m_lastLowPassCutoff = 0.0f;
}

bool AudioSystem::updateEffectParameters(const std::string& effectName, const IEffectParameters& parameters)
{
    std::string effectLower = toLowercase(effectName);
    
    for (auto& effect : m_effects)
    {
        if (effectLower == "delay" || effectLower == "echo") {
            if (auto delayEffect = std::dynamic_pointer_cast<DelayEffect>(effect)) {
                if (auto delayParams = dynamic_cast<const DelayParameters*>(&parameters)) {
                    delayEffect->setDelayTime(delayParams->delayTime);
                    delayEffect->setFeedback(delayParams->feedback);
                    delayEffect->setMix(delayParams->mix);
                    return true;
                }
            }
        }
        else if (effectLower == "lowpass" || effectLower == "lpf" || effectLower == "filter") {
            if (auto lowPassEffect = std::dynamic_pointer_cast<LowPassEffect>(effect)) {
                if (auto lowPassParams = dynamic_cast<const LowPassParameters*>(&parameters)) {
                    lowPassEffect->setCutoff(lowPassParams->cutoffFreq);
                    lowPassEffect->setResonance(lowPassParams->resonance);
                    m_lowPassActive = true;
                    m_lastLowPassCutoff = lowPassParams->cutoffFreq;
                    return true;
                }
            }
        }
        else if (effectLower == "octave") {
            if (auto octaveEffect = std::dynamic_pointer_cast<OctaveEffect>(effect)) {
                if (auto octaveParams = dynamic_cast<const OctaveParameters*>(&parameters)) {
                    // Convert octave shift to boolean (higher/lower)
                    bool higher = octaveParams->octaveShift > 1.0f;
                    octaveEffect->setHigher(higher);
                    octaveEffect->setBlend(octaveParams->mix);
                    return true;
                }
            }
        }
    }
    
    return false; // Effect not found or parameters don't match
}

void AudioSystem::updateADSRParameters(float attackTime, float decayTime, float sustainLevel, float releaseTime)
{
    // Create new envelope with updated parameters
    m_envelope = std::make_unique<ADSREnvelope>(attackTime, decayTime, sustainLevel, releaseTime);
}

void AudioSystem::setDriftParameters(float rateHz, float amountCents, float jitterCents)
{
    m_lfoRateHz = std::max(rateHz, 0.0f);
    m_lfoAmountCents = std::max(amountCents, 0.0f);
    m_noteJitterAmountCents = std::max(jitterCents, 0.0f);
}

void AudioSystem::setWaveformTapBuffer(StereoSampleRingBuffer* tap)
{
    m_waveformTap = tap;
}

void AudioSystem::setLowPassCutoff(float cutoffHz)
{
    bool updated = false;
    for (const auto& effect : m_effects)
    {
        if (auto lowPass = std::dynamic_pointer_cast<LowPassEffect>(effect))
        {
            lowPass->setCutoff(cutoffHz);
            updated = true;
        }
    }

    if (updated)
    {
        m_lowPassActive = true;
        m_lastLowPassCutoff = cutoffHz;
    }
    else
    {
        m_lowPassActive = false;
        m_lastLowPassCutoff = 0.0f;
    }
}

float AudioSystem::getLowPassCutoff() const
{
    return m_lowPassActive ? m_lastLowPassCutoff : 0.0f;
}

bool AudioSystem::hasLowPassEffect() const
{
    return m_lowPassActive;
}

void AudioSystem::configureSecondaryOscillator(bool enabled, float mix, float detuneCents, int octaveOffset)
{
    m_secondaryEnabled = enabled;
    if (!enabled)
    {
        m_secondaryMix = 0.0f;
        m_secondaryDetuneCents = 0.0f;
        m_secondaryOctaveOffset = 0;
        m_secondaryPhase = 0.0f;
        return;
    }

    m_secondaryMix = std::max(0.0f, std::min(1.0f, mix));
    m_secondaryDetuneCents = std::max(detuneCents, 0.0f);
    m_secondaryOctaveOffset = std::max(-2, std::min(2, octaveOffset));
}

void AudioSystem::setSecondaryWaveform(std::shared_ptr<IWave> waveform)
{
    if (waveform)
    {
        m_secondaryWaveform = waveform;
    }
}

void AudioSystem::setPitchBend(int value)
{
    const int clamped = std::max(-8192, std::min(8191, value));

    float normalized = 0.0f;
    if (clamped >= 0)
    {
        normalized = static_cast<float>(clamped) / 8191.0f;
    }
    else
    {
        normalized = static_cast<float>(clamped) / 8192.0f;
    }

    constexpr float semitoneRange = 1.0f; // +/- one semitone
    m_pitchBendCents = normalized * (semitoneRange * 100.0f);
}
