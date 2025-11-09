import React, { useState, useCallback, useRef } from 'react';
import { NoteCalculator } from '../../utils/NoteCalculator';
import './Keyboard.css';

/**
 * Keyboard component
 * Follows Single Responsibility Principle - only handles keyboard UI and note triggering
 * Interface Segregation Principle - receives only the callbacks it needs
 */
interface KeyboardProps {
  onNoteOn: (frequency: number) => void;
  onNoteOff: (frequency: number) => void;
  activeFrequencies: number[];
}

interface KeyInfo {
  midiNote: number;
  isBlack: boolean;
  name: string;
}

export const Keyboard: React.FC<KeyboardProps> = ({
  onNoteOn,
  onNoteOff,
  activeFrequencies
}) => {
  const pressedKeysRef = useRef<Set<number>>(new Set());
  const [pressedKeys, setPressedKeys] = useState<Set<number>>(new Set());

  const updatePressedKeys = useCallback(() => {
    setPressedKeys(new Set(pressedKeysRef.current));
  }, []);

  // Generate two octaves of keys starting from C3 (MIDI 48)
  const generateKeys = useCallback((): KeyInfo[] => {
    const keys: KeyInfo[] = [];
    const startNote = 48; // C3
    const numOctaves = 2;

    for (let i = 0; i < 12 * numOctaves; i++) {
      const midiNote = startNote + i;
      const noteIndex = midiNote % 12;
      const isBlack = [1, 3, 6, 8, 10].includes(noteIndex);
      const note = NoteCalculator.midiToNote(midiNote);
      
      keys.push({
        midiNote,
        isBlack,
        name: `${note.name}${note.octave}`
      });
    }

    return keys;
  }, []);

  const handleKeyDown = (midiNote: number) => {
    if (pressedKeysRef.current.has(midiNote)) {
      return;
    }
    const frequency = NoteCalculator.midiToFrequency(midiNote);
    pressedKeysRef.current.add(midiNote);
    updatePressedKeys();
    onNoteOn(frequency);
  };

  const handleKeyUp = (midiNote: number) => {
    if (!pressedKeysRef.current.has(midiNote)) {
      return;
    }
    pressedKeysRef.current.delete(midiNote);
    updatePressedKeys();
    const frequency = NoteCalculator.midiToFrequency(midiNote);
    onNoteOff(frequency);
  };

  const isFrequencyActive = useCallback(
    (frequency: number) => activeFrequencies.some(active => Math.abs(active - frequency) < 1e-3),
    [activeFrequencies]
  );

  const keys = generateKeys();
  const whiteKeys = keys.filter(k => !k.isBlack);
  const blackKeys = keys.filter(k => k.isBlack);

  return (
    <div className="keyboard">
      <div className="keyboard-keys">
        {whiteKeys.map((key) => {
          const frequency = NoteCalculator.midiToFrequency(key.midiNote);
          const isPressed = pressedKeys.has(key.midiNote) || isFrequencyActive(frequency);

          return (
            <div
              key={key.midiNote}
              className={`key white-key ${isPressed ? 'pressed' : ''}`}
              onMouseDown={() => handleKeyDown(key.midiNote)}
              onMouseUp={() => handleKeyUp(key.midiNote)}
              onMouseLeave={() => handleKeyUp(key.midiNote)}
            >
              <span className="key-label">{key.name}</span>
            </div>
          );
        })}
        {blackKeys.map((key) => {
          const whiteKeysBefore = whiteKeys.filter(wk => wk.midiNote < key.midiNote).length;
          const position = whiteKeysBefore * 60 - 20; // 60px white key width, -20px offset
          const frequency = NoteCalculator.midiToFrequency(key.midiNote);
          const isPressed = pressedKeys.has(key.midiNote) || isFrequencyActive(frequency);

          return (
            <div
              key={key.midiNote}
              className={`key black-key ${isPressed ? 'pressed' : ''}`}
              style={{ left: `${position}px` }}
              onMouseDown={() => handleKeyDown(key.midiNote)}
              onMouseUp={() => handleKeyUp(key.midiNote)}
              onMouseLeave={() => handleKeyUp(key.midiNote)}
            >
              <span className="key-label">{key.name}</span>
            </div>
          );
        })}
      </div>
    </div>
  );
};
