#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <string>
#include <limits>
#include "Effects/IEffect.h"
#include "Effects/EffectParameters.h"
#include "Waves/IWave.h"
#include "AudioConfig.h"
#include "Envelope/ADSREnvelope.h"

/**
 * @file audioSystem.h
 * @brief Main audio processing system that handles sound generation and effects processing
 * 
 * This class provides functionality for generating simple audio tones and
 * processing them through a chain of audio effects.
 */

/**
 * @class AudioSystem
 * @brief Core audio processing class that generates tones and applies effects
 * 
 * The AudioSystem class handles the generation of simple audio tones based on
 * frequency input and processes them through a configurable chain of audio effects.
 * It provides interfaces for triggering notes, managing effects, and retrieving
 * processed audio samples.
 */
class StereoSampleRingBuffer;

class AudioSystem
{
public:
    /**
     * @brief Constructs an AudioSystem with the specified sample rate
     * @param sampleRate The number of samples per second (Hz)
     */
    explicit AudioSystem(float sampleRate);

    /**
     * @brief Triggers a note with the specified frequency
     * @param newFrequency The frequency in Hz of the note to play
     */
    void triggerNote(float newFrequency);

    /**
     * @brief Releases a note, optionally targeting a specific frequency
     * @param frequency Frequency to release, or NaN to release all active notes
     */
    void triggerNoteOff(float frequency = std::numeric_limits<float>::quiet_NaN());

    /**
     * @brief Calculates and returns the next stereo audio sample
     * @return A pair of floats representing the left and right channel values
     */
    std::pair<float, float> getNextSample();

    /**
     * @brief Adds an audio effect to the processing chain
     * @param effect Shared pointer to an effect implementing the IEffect interface
     */
    void addEffect(std::shared_ptr<IEffect> effect);

    /**
     * @brief Processes a stereo sample through all added effects
     * @param stereoSample Input stereo sample (left and right channels)
     * @return The processed stereo sample after applying all effects
     */
    std::pair<float, float> applyEffects(std::pair<float, float> stereoSample);

    /**
     * @brief Resets all effects to their initial state (clears buffers)
     */
    void resetEffects();

    /**
     * @brief Removes all effects from the processing chain
     */
    void clearEffects();

    /**
     * @brief Sets the waveform generator
     * @param waveform Shared pointer to a waveform generator implementing the IWave interface
     */
    void setWaveform(std::shared_ptr<IWave> waveform);
    void configureSecondaryOscillator(bool enabled, float mix, float detuneCents, int octaveOffset);
    void setSecondaryWaveform(std::shared_ptr<IWave> waveform);
    void setPitchBend(int value);

    /**
     * @brief Apply a configuration to choose waveform and effect chain
     *
     * The configuration structure contains the name of the desired waveform
     * and an ordered list of effect identifiers. Any existing effects are
     * cleared and replaced based on this information.
     */
    void configure(const AudioConfig& config);

    /**
     * @brief Update effect parameters without recreating the effects chain
     * @param effectName Name of the effect to update
     * @param parameters Parameters to apply to the effect
     * @return true if the effect was found and updated, false otherwise
     */
    bool updateEffectParameters(const std::string& effectName, const IEffectParameters& parameters);

    /**
     * @brief Update ADSR envelope parameters
     * @param attackTime Attack time in seconds
     * @param decayTime Decay time in seconds
     * @param sustainLevel Sustain level [0.0-1.0]
     * @param releaseTime Release time in seconds
     */
    void updateADSRParameters(float attackTime, float decayTime, float sustainLevel, float releaseTime);

    /**
     * @brief Configure oscillator drift (low-frequency modulation) parameters
     * @param rateHz LFO rate in Hertz
     * @param amountCents Peak modulation depth in cents
     * @param jitterCents Random detune range per note (peak cents)
     */
    void setDriftParameters(float rateHz, float amountCents, float jitterCents);

    /**
     * @brief Attach a ring buffer tap to capture post-processed samples.
     * @param tap Pointer to a ring buffer owned elsewhere. Pass nullptr to disable.
     */
    void setWaveformTapBuffer(StereoSampleRingBuffer* tap);

    /**
     * @brief Update the cutoff of any active low-pass filter effect
     * @param cutoffHz New cutoff frequency in Hertz
     */
    void setLowPassCutoff(float cutoffHz);

    /**
     * @brief Retrieve the most recent low-pass filter cutoff.
     * @return Last applied cutoff frequency in Hertz, or 0 if no filter is active.
     */
    float getLowPassCutoff() const;

    /**
     * @brief Check if a low-pass effect is currently active in the chain.
     */
    bool hasLowPassEffect() const;

private:
    float m_frequency;                                ///< Current note frequency in Hz
    float m_sampleRate;                               ///< Audio sample rate in Hz
    float m_primaryPhase;                             ///< Current phase of the primary oscillator (0.0 to 1.0)
    float m_secondaryPhase;                           ///< Current phase of the secondary oscillator (0.0 to 1.0)
    bool m_noteOn;                                    ///< Flag indicating whether a note is currently playing
    std::vector<std::shared_ptr<IEffect>> m_effects;  ///< Chain of audio effects to apply
    std::shared_ptr<IWave> m_primaryWaveform;         ///< Primary waveform generator
    std::shared_ptr<IWave> m_secondaryWaveform;       ///< Secondary waveform generator
    std::unique_ptr<ADSREnvelope> m_envelope;         ///< ADSR envelope for amplitude modulation

    struct ActiveNote {
        float frequency;
        float detuneCents;
    };

    std::vector<ActiveNote> m_activeNotes;            ///< Stack of active note metadata for legato handling

    // Drift / LFO parameters for subtle analog-style modulation
    float m_lfoPhase;                 ///< Normalized [0,1) phase of the drift LFO
    float m_lfoRateHz;                ///< Drift LFO frequency in Hz
    float m_lfoAmountCents;           ///< Peak drift amount in cents
    float m_noteJitterAmountCents;    ///< Random detune range applied per note in cents
    float m_noteDetuneCents;          ///< Random detune assigned to the current note

    StereoSampleRingBuffer* m_waveformTap;            ///< Optional capture buffer for visualization

    bool m_secondaryEnabled;                          ///< Whether the secondary oscillator is active
    float m_secondaryMix;                             ///< Mix amount for the secondary oscillator [0.0-1.0]
    float m_secondaryDetuneCents;                     ///< Positive detune amount applied to the secondary oscillator
    int m_secondaryOctaveOffset;                      ///< Octave shift applied to the secondary oscillator (-2 to +2)
    float m_pitchBendCents;                           ///< Pitch bend offset in cents (-50 to +50)

    bool m_lowPassActive;                             ///< Whether a low-pass effect is present in the chain
    float m_lastLowPassCutoff;                        ///< Last applied low-pass cutoff frequency
};
