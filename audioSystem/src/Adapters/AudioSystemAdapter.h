#pragma once

#include "IObserver.h"
#include "audioSystem.h"
#include "MidiEvent.h"

/**
 * @class AudioSystemAdapter
 * @brief Adapter that connects the audio system to the observer 
 *
 * This class implements the Adapter design pattern to allow an AudioSystem
 * instance to receive notifications through the Observer pattern. It wraps
 * an AudioSystem object and translates incoming notifications (MIDI events)
 * into appropriate calls to the AudioSystem.
 *
 * The adapter serves as a bridge between components that emit events and
 * the audio system that needs to respond to those events, decoupling the
 * event sources from the audio processing logic.
 */
class AudioSystemAdapter : public IObserver 
{
public:

    /**
     * @brief Constructor for AudioSystemAdapter
     * 
     * Creates an adapter that wraps an AudioSystem object and implements the IObserver
     * interface to receive notifications from observable subjects.
     * 
     * @param pAudioSystem Pointer to the AudioSystem instance to be adapted
     * @pre pAudioSystem must not be nullptr
     */
    AudioSystemAdapter  (AudioSystem* pAudioSystem);

    /**
     * @brief Handles notifications from observed subjects
     *
     * This method is called when a subject this adapter is observing notifies
     * its observers of a change. The method interprets the parameters and
     * forwards appropriate commands to the underlying AudioSystem.
     *
     * @param params A pointer to the data associated with the notification,
     *               typically a MidiEvent or similar structure
     */
    void update         (void* params) override;
    
private:

    AudioSystem* itsAudioSystem;    ///< Pointer to the adapted AudioSystem instance
};
