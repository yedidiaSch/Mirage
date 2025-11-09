import React, { useState, useEffect, useRef } from 'react';
import { AudioService } from '../../services/AudioService';
import { WaveformSelector } from './WaveformSelector';
import { EnvelopeControls } from './EnvelopeControls';
import { Keyboard } from './Keyboard';
import { EffectsPanel, EffectSettings } from './EffectsPanel';
import { DriftControls, DriftSettings } from './DriftControls';
import { MidiStatus } from './MidiStatus';
import { WaveformDisplay } from './WaveformDisplay';
import { SynthState, SecondaryOscillatorSettings } from '../../types';
import { SecondaryOscillatorControls } from './SecondaryOscillatorControls';
import './shared/Panel.css';
import './Synthesizer.css';

const DEFAULT_DRIFT_SETTINGS: DriftSettings = {
  rate: 0.35,
  amount: 4,
  jitter: 3
};

const DEFAULT_SECONDARY_OSC: SecondaryOscillatorSettings = {
  enabled: false,
  mix: 0.35,
  detuneCents: 7,
  octaveOffset: 0
};

/**
 * Main Synthesizer component
 * Orchestrates the audio engine and UI components
 * Follows Open/Closed Principle - extensible through child components
 */
export const Synthesizer: React.FC = () => {
  const [audioService] = useState(() => new AudioService(44100));
  const [initError, setInitError] = useState<string | null>(null);
  const effectsTimeoutRef = useRef<NodeJS.Timeout | null>(null);
  const [synthState, setSynthState] = useState<SynthState>({
    isPlaying: false,
    currentFrequency: null,
    activeFrequencies: [],
    waveform: 'sine',
    adsr: {
      attack: 0.01,
      decay: 0.1,
      sustain: 0.7,
      release: 0.3
    },
    secondaryOscillator: { ...DEFAULT_SECONDARY_OSC }
  });
  const [effectSettings, setEffectSettings] = useState<EffectSettings>({
    delay: {
      enabled: false,
      time: 0.3,
      feedback: 0.5,
      mix: 0.5
    },
    lowPass: {
      enabled: false,
      cutoff: 1200,
      resonance: 1.2
    }
  });
  const [driftSettings, setDriftSettings] = useState<DriftSettings>({ ...DEFAULT_DRIFT_SETTINGS });

  // Sync low-pass cutoff with incoming MIDI CC updates so the UI reflects hardware control changes
  useEffect(() => {
    if (!audioService.initialized) {
      return;
    }

  const hasGetter = typeof audioService.getLowPassCutoff === 'function';
    if (!hasGetter) {
      return;
    }

    if (!effectSettings.lowPass.enabled) {
      return;
    }

    const interval = setInterval(() => {
      try {
        const cutoffValue = audioService.getLowPassCutoff();
        if (!Number.isFinite(cutoffValue) || cutoffValue <= 0) {
          return;
        }

        setEffectSettings(prev => {
          if (!prev.lowPass.enabled) {
            return prev;
          }

          if (Math.abs(prev.lowPass.cutoff - cutoffValue) < 1) {
            return prev;
          }

          return {
            ...prev,
            lowPass: {
              ...prev.lowPass,
              cutoff: cutoffValue
            }
          };
        });
      } catch (error) {
        console.error('Failed to sync low-pass cutoff from MIDI control change:', error);
      }
    }, 120);

    return () => clearInterval(interval);
  }, [audioService, effectSettings.lowPass.enabled]);

  // Initialize audio service on mount, cleanup on unmount
  useEffect(() => {
    console.log('Synthesizer: Attempting to initialize audio service...');
    try {
      audioService.initialize();
      console.log('Synthesizer: Audio service initialized successfully');
      setInitError(null);
      audioService.updateDrift(
        DEFAULT_DRIFT_SETTINGS.rate,
        DEFAULT_DRIFT_SETTINGS.amount,
        DEFAULT_DRIFT_SETTINGS.jitter
      );
      audioService.configureSecondaryOscillator(DEFAULT_SECONDARY_OSC);
    } catch (error) {
      const errorMsg = `Failed to initialize audio: ${error}`;
      console.error('Synthesizer:', errorMsg);
      setInitError(errorMsg);
    }

    // Cleanup: shutdown audio when component unmounts
    return () => {
      console.log('Synthesizer: Cleaning up audio service...');
      audioService.shutdown();
    };
  }, [audioService]);

  // Push drift settings into the audio engine whenever they change
  useEffect(() => {
    if (!audioService.initialized) {
      return;
    }

    audioService.updateDrift(driftSettings.rate, driftSettings.amount, driftSettings.jitter);
  }, [audioService, driftSettings.rate, driftSettings.amount, driftSettings.jitter]);

  useEffect(() => {
    if (!audioService.initialized) {
      return;
    }

    audioService.configureSecondaryOscillator(synthState.secondaryOscillator);
  }, [audioService, synthState.secondaryOscillator]);

  const handleDriftChange = (settings: DriftSettings) => {
    setDriftSettings(settings);
  };

  const handleNoteOn = (frequency: number) => {
    audioService.playNote(frequency);
    const activeFrequencies = audioService.getActiveFrequencies();
    setSynthState(prev => ({
      ...prev,
      isPlaying: activeFrequencies.length > 0,
      currentFrequency: frequency,
      activeFrequencies
    }));
  };

  const handleNoteOff = (frequency: number) => {
    audioService.stopNote(frequency);
    const activeFrequencies = audioService.getActiveFrequencies();
    setSynthState(prev => ({
      ...prev,
      isPlaying: activeFrequencies.length > 0,
      currentFrequency: activeFrequencies.length ? activeFrequencies[activeFrequencies.length - 1] : null,
      activeFrequencies
    }));
  };

  const handleWaveformChange = (waveform: SynthState['waveform']) => {
    audioService.setWaveform(waveform);
    setSynthState(prev => ({ ...prev, waveform }));
  };

  const handleEnvelopeChange = (adsr: SynthState['adsr']) => {
    audioService.updateEnvelope(adsr);
    setSynthState(prev => ({ ...prev, adsr }));
  };

  const handleEffectsChange = (settings: EffectSettings) => {
    setEffectSettings(settings);

    // Clear existing timeout
    if (effectsTimeoutRef.current) {
      clearTimeout(effectsTimeoutRef.current);
    }
    
    // Immediately log the change
    console.log('Effect settings changed:', {
      delay: settings.delay.enabled ? 'ON' : 'OFF',
      lowPass: settings.lowPass.enabled ? 'ON' : 'OFF'
    });
    
    // Debounce effect application to avoid rebuilding on every slider change
    // But still apply within 100ms for responsive feel
    effectsTimeoutRef.current = setTimeout(() => {
      console.log('Applying effects to audio system...');
      audioService.applyEffects(settings);
    }, 100); // 100ms debounce
  };

  const handleSecondaryOscChange = (updates: Partial<SecondaryOscillatorSettings>) => {
    setSynthState(prev => ({
      ...prev,
      secondaryOscillator: {
        ...prev.secondaryOscillator,
        ...updates
      }
    }));
  };

  const formatFrequency = (frequency: number) => {
    if (!Number.isFinite(frequency) || frequency <= 0) {
      return null;
    }

    const midi = Math.round(69 + 12 * Math.log2(frequency / 440));
    const noteNames = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
    const noteName = noteNames[((midi % 12) + 12) % 12];
    const octave = Math.floor(midi / 12) - 1;
    return `${noteName}${octave} · ${frequency.toFixed(1)} Hz`;
  };

  return (
    <div className="synthesizer">
      <div className="synth-header">
        <div className="synth-title">
          <span className="synth-badge">SynthUI</span>
          <h1>Analog Control Station</h1>
          <p>Dual oscillator hybrid desk with live visualization</p>
        </div>
        <MidiStatus audioService={audioService} />
      </div>

      {initError && (
        <div className="synth-error">
          <strong>Audio Initialization Error:</strong> {initError}
        </div>
      )}

      <div className="synth-layout">
        <div className="synth-column">
          <div className="panel-shell">
            <WaveformSelector
              selectedWaveform={synthState.waveform}
              onWaveformChange={handleWaveformChange}
            />
          </div>
          <div className="panel-shell panel-shell--accent">
            <SecondaryOscillatorControls
              settings={synthState.secondaryOscillator}
              onChange={handleSecondaryOscChange}
            />
          </div>
          <div className="panel-shell panel-shell--flush">
            <DriftControls
              settings={driftSettings}
              onChange={handleDriftChange}
            />
          </div>
        </div>

        <div className="synth-column">
          <div className="panel-shell">
            <EnvelopeControls
              adsr={synthState.adsr}
              onEnvelopeChange={handleEnvelopeChange}
            />
          </div>
          <div className="panel-shell panel-shell--flush">
            <EffectsPanel
              settings={effectSettings}
              onSettingsChange={handleEffectsChange}
            />
          </div>
        </div>

        <div className="synth-column">
          <div className="panel-shell panel-shell--meter oscilloscope-panel">
            <div className="panel-shell__title"><strong>Oscilloscope</strong></div>
            <WaveformDisplay audioService={audioService} refreshMs={120} maxFrames={4096} />
          </div>
        </div>
      </div>

      <div className="panel-shell keyboard-panel">
        <div className="panel-shell__title"><strong>Performance Keyboard</strong></div>
        <Keyboard
          onNoteOn={handleNoteOn}
          onNoteOff={handleNoteOff}
          activeFrequencies={synthState.activeFrequencies}
        />
      </div>
    </div>
  );
};
