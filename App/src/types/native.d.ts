/**
 * Type definitions for the native AudioSystem module
 */

export interface StereoSample {
  left: number;
  right: number;
}

export type WaveformType = 'sine' | 'square' | 'saw' | 'triangle';

export interface ADSRParameters {
  attack: number;
  decay: number;
  sustain: number;
  release: number;
}

/**
 * Native AudioSystem interface exposed through N-API
 */
export interface IAudioSystemNative {
  /**
   * Start the audio output stream
   */
  start(): void;

  /**
   * Stop the audio output stream
   */
  stop(): void;

  /**
   * Trigger a note with the specified frequency
   * @param frequency - Frequency in Hz
   */
  triggerNote(frequency: number): void;

  /**
   * Stop notes, optionally targeting a specific frequency
   * @param frequency - Frequency in Hz to release, omit to release all
   */
  triggerNoteOff(frequency?: number): void;

  /**
   * Reset all audio effects (clears internal state without removing from chain)
   */
  resetEffects(): void;

  /**
   * Clear all audio effects from the processing chain
   */
  clearEffects(): void;

  /**
   * Update ADSR envelope parameters
   * @param attack - Attack time in seconds
   * @param decay - Decay time in seconds
   * @param sustain - Sustain level (0.0 - 1.0)
   * @param release - Release time in seconds
   */
  updateADSRParameters(attack: number, decay: number, sustain: number, release: number): void;

  /**
   * Set the waveform type
   * @param waveformType - Type of waveform to use
   */
  setWaveform(waveformType: WaveformType): void;

  /**
   * Add a delay effect to the effects chain
   * @param delayTime - Delay time in seconds (0.0 - 2.0)
   * @param feedback - Feedback amount (0.0 - 1.0)
   * @param mix - Wet/dry mix (0.0 - 1.0)
   */
  addDelayEffect(delayTime: number, feedback: number, mix: number): void;

  /**
   * Add a low-pass filter to the effects chain
   * @param cutoff - Cutoff frequency in Hz
   */
  addLowPassEffect(cutoff: number, resonance?: number, mix?: number): void;

  /**
   * Update the cutoff frequency of any active low-pass filter
   * @param cutoff - Cutoff frequency in Hz
   */
  setLowPassCutoff(cutoff: number): void;

  /**
   * Retrieve the last applied low-pass cutoff frequency.
   */
  getLowPassCutoff(): number;

  /**
   * Add an octave effect to the effects chain
   * @param higher - true for higher octave, false for lower
   * @param blend - Blend amount (0.0 - 1.0)
   */
  addOctaveEffect(higher: boolean, blend: number): void;

  /**
   * Configure oscillator drift / LFO parameters
   * @param rateHz - Speed of the LFO in Hertz
   * @param amountCents - Peak modulation depth in cents
   * @param jitterCents - Per-note random detune range in cents
   */
  setDriftParameters(rateHz: number, amountCents: number, jitterCents: number): void;
  configureSecondaryOscillator(enabled: boolean, mix: number, detuneCents: number, octaveOffset: number): void;
  setPitchBend(value: number): void;

  /**
   * Get MIDI device connection status
   * @returns Object with connection status and device name
   */
  getMidiStatus(): { connected: boolean; deviceName: string };

  /**
   * Copy the most recent interleaved stereo samples for visualization.
   * @param maxFrames Maximum number of frames to retrieve.
   * @returns Float32Array with interleaved left/right samples.
   */
  getRecentWaveform(maxFrames?: number): Float32Array;
}

/**
 * Native module constructor
 */
export interface IAudioSystemNativeConstructor {
  new (sampleRate: number): IAudioSystemNative;
}

export interface IAudioSystemNativeModule {
  AudioSystem: IAudioSystemNativeConstructor;
}
