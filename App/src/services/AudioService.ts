import { IAudioSystemNative, IAudioSystemNativeModule, WaveformType, ADSRParameters } from '../types/native';
import { SecondaryOscillatorSettings } from '../types';

/**
 * @class AudioService
 * @brief Service layer that manages the native audio system
 * 
 * This class follows the Single Responsibility Principle by handling
 * only the JavaScript-level interaction with the native audio module.
 * It provides a clean API for the application layer.
 */
export class AudioService {
  private audioSystem: IAudioSystemNative | null = null;
  private readonly sampleRate: number;
  private isInitialized: boolean = false;
  private readonly activeFrequencies: Set<number> = new Set();

  constructor(sampleRate: number = 44100) {
    this.sampleRate = sampleRate;
  }

  /**
   * Initialize the audio system
   * Lazy loading to ensure native module is available
   */
  public initialize(): void {
    if (this.isInitialized) {
      return;
    }

    try {
      // In Electron, __dirname in bundled code points to dist/renderer
      // The native module is copied to dist/ during build
      // Direct require to avoid webpack bundling issues
      console.log('Loading native module from: ../audioSystemNative.node (relative to dist/renderer)');
      const nativeModule: IAudioSystemNativeModule = require('../audioSystemNative.node');
      
    this.audioSystem = new nativeModule.AudioSystem(this.sampleRate);
    this.audioSystem.start(); // Start the audio output stream
    this.isInitialized = true;
    this.activeFrequencies.clear();
    console.log('✓ Audio system initialized successfully');
    } catch (error) {
      console.error('✗ Failed to initialize audio system:', error);
      throw new Error(`Failed to initialize audio system: ${error}`);
    }
  }

  /**
   * Cleanup and stop audio
   */
  public shutdown(): void {
    if (this.audioSystem) {
      this.audioSystem.stop();
      this.activeFrequencies.clear();
      this.isInitialized = false;
    }
  }

  /**
   * Play a note at the specified frequency
   */
  public playNote(frequency: number): void {
    this.ensureInitialized();
    this.audioSystem!.triggerNote(frequency);
    if (Number.isFinite(frequency)) {
      this.activeFrequencies.add(frequency);
    }
  }

  /**
   * Stop the currently playing note
   */
  public stopNote(frequency?: number): void {
    this.ensureInitialized();
    if (typeof frequency === 'number' && Number.isFinite(frequency)) {
      this.activeFrequencies.delete(frequency);
      this.audioSystem!.triggerNoteOff(frequency);
    } else {
      this.activeFrequencies.clear();
      this.audioSystem!.triggerNoteOff();
    }
  }

  /**
   * Set the waveform type
   */
  public setWaveform(waveform: WaveformType): void {
    this.ensureInitialized();
    this.audioSystem!.setWaveform(waveform);
  }

  /**
   * Update ADSR envelope parameters
   */
  public updateEnvelope(params: ADSRParameters): void {
    this.ensureInitialized();
    this.audioSystem!.updateADSRParameters(
      params.attack,
      params.decay,
      params.sustain,
      params.release
    );
  }

  /**
   * Reset all effects
   */
  public resetEffects(): void {
    this.ensureInitialized();
    this.audioSystem!.resetEffects();
  }

  /**
   * Apply effects based on settings
   * This rebuilds the effect chain by clearing and adding enabled effects
   */
  public applyEffects(settings: {
    delay?: { enabled: boolean; time: number; feedback: number; mix: number };
    lowPass?: { enabled: boolean; cutoff: number; resonance?: number };
  }): void {
    this.ensureInitialized();
    
    console.log('=== Rebuilding Effects Chain ===');
    
    // Clear the effects chain first (removes ALL effects)
    this.audioSystem!.clearEffects();
    console.log('✓ All effects cleared');

    // Add effects in order based on what's enabled
    let effectCount = 0;
    
    if (settings.delay?.enabled) {
      console.log('✓ Adding Delay:', {
        time: settings.delay.time.toFixed(3) + 's',
        feedback: (settings.delay.feedback * 100).toFixed(0) + '%',
        mix: (settings.delay.mix * 100).toFixed(0) + '%'
      });
      this.audioSystem!.addDelayEffect(
        settings.delay.time,
        settings.delay.feedback,
        settings.delay.mix
      );
      effectCount++;
    } else {
      console.log('✗ Delay: disabled');
    }

    if (settings.lowPass?.enabled) {
      const resonance = settings.lowPass.resonance ?? 1.0;
      console.log('✓ Adding Low-Pass Filter:', {
        cutoff: settings.lowPass.cutoff.toFixed(0) + ' Hz',
        resonance: resonance.toFixed(2)
      });
      this.audioSystem!.addLowPassEffect(settings.lowPass.cutoff, resonance);
      this.audioSystem!.setLowPassCutoff(settings.lowPass.cutoff);
      effectCount++;
    } else {
      console.log('✗ Low-Pass Filter: disabled');
    }
    
    console.log(`=== Effects chain complete: ${effectCount} effect(s) active ===`);
  }

  /**
   * Check if the service is initialized
   */
  public get initialized(): boolean {
    return this.isInitialized;
  }

  /**
   * Update oscillator drift parameters
   */
  public updateDrift(rateHz: number, amountCents: number, jitterCents: number): void {
    this.ensureInitialized();
    this.audioSystem!.setDriftParameters(rateHz, amountCents, jitterCents);
  }

  /**
   * Directly update the low-pass cutoff
   */
  public setLowPassCutoff(cutoff: number): void {
    this.ensureInitialized();
    this.audioSystem!.setLowPassCutoff(cutoff);
  }

  /**
   * Query the current low-pass cutoff.
   */
  public getLowPassCutoff(): number {
    this.ensureInitialized();
    return this.audioSystem!.getLowPassCutoff();
  }

  /**
   * Get MIDI device status
   */
  public getMidiStatus(): { connected: boolean; deviceName: string } {
    this.ensureInitialized();
    return this.audioSystem!.getMidiStatus();
  }

  /**
   * Retrieve the most recent waveform samples for visualization.
   */
  public getWaveformData(frameCount: number = 2048): Float32Array {
    if (!this.isInitialized || !this.audioSystem) {
      return new Float32Array(0);
    }

    if (!Number.isFinite(frameCount) || frameCount <= 0) {
      return new Float32Array(0);
    }

    const clampedFrames = Math.min(Math.floor(frameCount), this.sampleRate);
    return this.audioSystem.getRecentWaveform(clampedFrames);
  }

  /**
   * Configure the secondary oscillator mix and detune parameters.
   */
  public configureSecondaryOscillator(settings: SecondaryOscillatorSettings): void {
    this.ensureInitialized();

    const enabled = Boolean(settings.enabled);
    const mix = Math.max(0, Math.min(1, settings.mix));
    const detune = Math.max(0, settings.detuneCents);
    const octave = Math.max(-2, Math.min(2, Math.round(settings.octaveOffset)));

    this.audioSystem!.configureSecondaryOscillator(enabled, mix, detune, octave);
  }

  /**
   * Apply raw pitch bend value from MIDI pitch wheel (-8192 to +8191).
   * Scaled internally to +/- 0.5 semitones.
   */
  public setPitchBend(rawValue: number): void {
    this.ensureInitialized();

    if (!Number.isFinite(rawValue)) {
      return;
    }

    const clamped = Math.max(-8192, Math.min(8191, Math.round(rawValue)));
    this.audioSystem!.setPitchBend(clamped);
  }

  /**
   * Get a snapshot of currently active note frequencies
   */
  public getActiveFrequencies(): number[] {
    this.ensureInitialized();
    return Array.from(this.activeFrequencies);
  }

  private ensureInitialized(): void {
    if (!this.isInitialized || !this.audioSystem) {
      throw new Error('AudioService not initialized. Call initialize() first.');
    }
  }
}
