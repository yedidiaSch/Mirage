import { Note } from '../types';

/**
 * @class NoteCalculator
 * @brief Utility for calculating note frequencies
 * 
 * Follows Single Responsibility Principle - only handles note frequency calculations
 */
export class NoteCalculator {
  private static readonly A4_FREQUENCY = 440;
  private static readonly A4_MIDI_NUMBER = 69;

  /**
   * Calculate frequency from MIDI note number
   * @param midiNote - MIDI note number (0-127)
   * @returns Frequency in Hz
   */
  public static midiToFrequency(midiNote: number): number {
    return this.A4_FREQUENCY * Math.pow(2, (midiNote - this.A4_MIDI_NUMBER) / 12);
  }

  /**
   * Convert note name and octave to frequency
   * @param noteName - Note name (C, C#, D, etc.)
   * @param octave - Octave number
   * @returns Frequency in Hz
   */
  public static noteToFrequency(noteName: string, octave: number): number {
    const noteMap: Record<string, number> = {
      'C': 0, 'C#': 1, 'D': 2, 'D#': 3, 'E': 4, 'F': 5,
      'F#': 6, 'G': 7, 'G#': 8, 'A': 9, 'A#': 10, 'B': 11
    };

    const noteOffset = noteMap[noteName.toUpperCase()];
    if (noteOffset === undefined) {
      throw new Error(`Invalid note name: ${noteName}`);
    }

    const midiNote = (octave + 1) * 12 + noteOffset;
    return this.midiToFrequency(midiNote);
  }

  /**
   * Generate a note object from MIDI number
   */
  public static midiToNote(midiNote: number): Note {
    const noteNames = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
    const octave = Math.floor(midiNote / 12) - 1;
    const noteName = noteNames[midiNote % 12];
    const frequency = this.midiToFrequency(midiNote);

    return { frequency, name: noteName, octave };
  }
}
