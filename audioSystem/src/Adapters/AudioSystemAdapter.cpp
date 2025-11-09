#include "AudioSystemAdapter.h"
#include "Common/notes.h"
#include <cmath>
#include <stdexcept>

AudioSystemAdapter::AudioSystemAdapter(AudioSystem* pAudioSystem) : itsAudioSystem(pAudioSystem) 
{
    if (itsAudioSystem == nullptr) {
        throw std::invalid_argument("AudioSystem pointer cannot be null");
    }
}


void AudioSystemAdapter::update(void* params) 
{
    if (params == nullptr) {
        throw std::invalid_argument("Params pointer cannot be null");
    }
    
    // Cast params to MidiEvent
    const MidiEvent* event = static_cast<const MidiEvent*>(params);
    
    switch (event->type) {
        case MidiEventType::NOTE_ON:
        {
            const unsigned char noteNumber = event->data1;
            if (noteNumber < MIDI_NOTE_FREQUENCIES.size()) {
                itsAudioSystem->triggerNote(MIDI_NOTE_FREQUENCIES[noteNumber]);
            }
            break;
        }
        case MidiEventType::NOTE_OFF:
        {
            const unsigned char noteNumber = event->data1;
            if (noteNumber < MIDI_NOTE_FREQUENCIES.size()) {
                itsAudioSystem->triggerNoteOff(MIDI_NOTE_FREQUENCIES[noteNumber]);
            } else {
                itsAudioSystem->triggerNoteOff();
            }
            break;
        }
        case MidiEventType::PITCH_BEND:
        {
            itsAudioSystem->setPitchBend(event->value);
            break;
        }
        case MidiEventType::CONTROL_CHANGE:
        {
            if (event->data1 == 7)
            {
                constexpr float minCutoff = 80.0f;
                constexpr float maxCutoff = 12000.0f;
                const float normalized = static_cast<float>(event->data2) / 127.0f;
                const float ratio = maxCutoff / minCutoff;
                const float cutoff = minCutoff * std::pow(ratio, normalized);
                itsAudioSystem->setLowPassCutoff(cutoff);
            }
            break;
        }
            
        // Handle other event types as needed
        default:
            break;
    }
}
