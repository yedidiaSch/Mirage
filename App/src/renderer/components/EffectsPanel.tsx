import React from 'react';
import './EffectsPanel.css';
import { Knob } from './controls/Knob';

export interface EffectSettings {
  delay: {
    enabled: boolean;
    time: number;      // 0.0 - 2.0 seconds
    feedback: number;  // 0.0 - 1.0
    mix: number;       // 0.0 - 1.0
  };
  lowPass: {
    enabled: boolean;
    cutoff: number;    // 100 - 10000 Hz
    resonance: number; // 0.1 - 4.0
  };
  // Octave effect disabled - needs proper pitch shifting implementation
  // octave: {
  //   enabled: boolean;
  //   higher: boolean;
  //   blend: number;
  // };
}

interface EffectsPanelProps {
  settings: EffectSettings;
  onSettingsChange: (settings: EffectSettings) => void;
}

/**
 * @component EffectsPanel
 * @brief UI panel for controlling audio effects
 * 
 * Provides controls for Delay and Low-Pass Filter effects.
 * Each effect can be toggled on/off and has specific parameters.
 */
export const EffectsPanel: React.FC<EffectsPanelProps> = ({ settings, onSettingsChange }) => {
  
  const handleDelayToggle = () => {
    onSettingsChange({
      ...settings,
      delay: { ...settings.delay, enabled: !settings.delay.enabled }
    });
  };

  const handleDelayChange = (param: 'time' | 'feedback' | 'mix', value: number) => {
    onSettingsChange({
      ...settings,
      delay: { ...settings.delay, [param]: value }
    });
  };

  const handleLowPassToggle = () => {
    onSettingsChange({
      ...settings,
      lowPass: { ...settings.lowPass, enabled: !settings.lowPass.enabled }
    });
  };

  const handleLowPassChange = (param: 'cutoff' | 'resonance', value: number) => {
    onSettingsChange({
      ...settings,
      lowPass: { ...settings.lowPass, [param]: value }
    });
  };

  return (
    <div className="effects-panel">
      <h3>Effects</h3>
      
      {/* Delay Effect */}
      <div className={`effect-section ${settings.delay.enabled ? 'enabled' : 'disabled'}`}>
        <div className="effect-header">
          <label>
            <input
              type="checkbox"
              checked={settings.delay.enabled}
              onChange={handleDelayToggle}
            />
            <span className="effect-name">Delay</span>
          </label>
        </div>
        <div className="effect-controls">
          <Knob
            label="Time"
            min={0.01}
            max={2.0}
            step={0.01}
            value={settings.delay.time}
            unit="s"
            onChange={(value) => handleDelayChange('time', value)}
            disabled={!settings.delay.enabled}
            formatValue={(v) => v.toFixed(2)}
          />
          <Knob
            label="Feedback"
            min={0}
            max={1}
            step={0.01}
            value={settings.delay.feedback}
            onChange={(value) => handleDelayChange('feedback', value)}
            disabled={!settings.delay.enabled}
            formatValue={(v) => `${Math.round(v * 100)}`}
            unit="%"
          />
          <Knob
            label="Mix"
            min={0}
            max={1}
            step={0.01}
            value={settings.delay.mix}
            onChange={(value) => handleDelayChange('mix', value)}
            disabled={!settings.delay.enabled}
            formatValue={(v) => `${Math.round(v * 100)}`}
            unit="%"
          />
        </div>
      </div>

      {/* Low-Pass Filter */}
      <div className={`effect-section ${settings.lowPass.enabled ? 'enabled' : 'disabled'}`}>
        <div className="effect-header">
          <label>
            <input
              type="checkbox"
              checked={settings.lowPass.enabled}
              onChange={handleLowPassToggle}
            />
            <span className="effect-name">Low-Pass Filter</span>
          </label>
        </div>
        <div className="effect-controls">
          <Knob
            label="Cutoff"
            min={100}
            max={10000}
            step={10}
            value={settings.lowPass.cutoff}
            unit="Hz"
            onChange={(value) => handleLowPassChange('cutoff', value)}
            disabled={!settings.lowPass.enabled}
            formatValue={(v) => `${Math.round(v)}`}
          />
          <Knob
            label="Resonance"
            min={0.1}
            max={4.0}
            step={0.01}
            value={settings.lowPass.resonance}
            onChange={(value) => handleLowPassChange('resonance', value)}
            disabled={!settings.lowPass.enabled}
            formatValue={(v) => v.toFixed(2)}
          />
        </div>
      </div>
    </div>
  );
};
