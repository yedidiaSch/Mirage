/**
 * Application-level types and interfaces
 */

export interface Note {
  frequency: number;
  name: string;
  octave: number;
}

export interface SynthState {
  isPlaying: boolean;
  currentFrequency: number | null;
  activeFrequencies: number[];
  waveform: 'sine' | 'square' | 'saw' | 'triangle';
  adsr: {
    attack: number;
    decay: number;
    sustain: number;
    release: number;
  };
  secondaryOscillator: SecondaryOscillatorSettings;
}

export interface SecondaryOscillatorSettings {
  enabled: boolean;
  mix: number; // 0.0 - 1.0
  detuneCents: number; // >= 0
  octaveOffset: number; // -2 to +2
}
