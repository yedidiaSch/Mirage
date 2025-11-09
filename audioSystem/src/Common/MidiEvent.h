#pragma once

/**
 * @file MidiEvent.h
 * @brief Defines MIDI event types and structure for MIDI message handling
 *
 * This header provides the fundamental types and structures needed to represent
 * MIDI events in the audio processing system.
 */

/**
 * @enum MidiEventType
 * @brief Defines the types of MIDI events supported by the system
 */
enum class MidiEventType {
    NOTE_ON,        ///< Note-on event (key pressed)
    NOTE_OFF,       ///< Note-off event (key released)
    CONTROL_CHANGE, ///< Control change event (knob/slider moved)
    PITCH_BEND      ///< Pitch bend event (pitch wheel moved)
};

/**
 * @struct MidiEvent
 * @brief Represents a single MIDI event with all relevant data
 *
 * This structure encapsulates all information needed to process a MIDI message,
 * including its type, channel, and associated data bytes.
 */
struct MidiEvent {
    MidiEventType type;    ///< Type of MIDI event
    unsigned char channel; ///< MIDI channel (0-15)
    unsigned char data1;   ///< First data byte: Note number (0-127) or controller number (0-127)
    unsigned char data2;   ///< Second data byte: Velocity (0-127) or controller value (0-127)
    int value;             ///< Combined value for pitch bend (-8192 to +8191) or other multi-byte data
};
