import React from 'react';
import { SecondaryOscillatorSettings } from '../../types';
import './SecondaryOscillatorControls.css';

interface SecondaryOscillatorControlsProps {
  settings: SecondaryOscillatorSettings;
  onChange: (updates: Partial<SecondaryOscillatorSettings>) => void;
}

const clamp = (value: number, min: number, max: number) => Math.min(Math.max(value, min), max);

export const SecondaryOscillatorControls: React.FC<SecondaryOscillatorControlsProps> = ({
  settings,
  onChange
}) => {
  const handleEnabledToggle = (event: React.ChangeEvent<HTMLInputElement>) => {
    const enabled = event.target.checked;
    onChange({ enabled });
  };

  const handleMixChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const mixPercent = clamp(Number(event.target.value), 0, 100);
    onChange({ mix: mixPercent / 100 });
  };

  const handleDetuneChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const detune = clamp(Number(event.target.value), 0, 100);
    onChange({ detuneCents: detune });
  };

  const handleOctaveChange = (event: React.ChangeEvent<HTMLSelectElement>) => {
    const offset = clamp(Number(event.target.value), -2, 2);
    onChange({ octaveOffset: offset });
  };

  return (
    <div className="secondary-oscillator">
      <div className="secondary-oscillator__header">
        <h3>Oscillator 2</h3>
        <label className="toggle">
          <input
            type="checkbox"
            checked={settings.enabled}
            onChange={handleEnabledToggle}
          />
          <span>Enabled</span>
        </label>
      </div>

      <div className="secondary-oscillator__body">
        <div className="secondary-oscillator__control">
          <label htmlFor="osc2-mix">Mix</label>
          <input
            id="osc2-mix"
            type="range"
            min={0}
            max={100}
            step={1}
            value={Math.round(clamp(settings.mix, 0, 1) * 100)}
            onChange={handleMixChange}
            disabled={!settings.enabled}
          />
          <span className="value">{Math.round(clamp(settings.mix, 0, 1) * 100)}%</span>
        </div>

        <div className="secondary-oscillator__control">
          <label htmlFor="osc2-detune">Detune (cents)</label>
          <input
            id="osc2-detune"
            type="range"
            min={0}
            max={100}
            step={1}
            value={Math.round(clamp(settings.detuneCents, 0, 100))}
            onChange={handleDetuneChange}
            disabled={!settings.enabled}
          />
          <span className="value">+{Math.round(clamp(settings.detuneCents, 0, 100))}</span>
        </div>

        <div className="secondary-oscillator__control">
          <label htmlFor="osc2-octave">Octave</label>
          <select
            id="osc2-octave"
            value={clamp(settings.octaveOffset, -2, 2)}
            onChange={handleOctaveChange}
            disabled={!settings.enabled}
          >
            <option value={-2}>-2</option>
            <option value={-1}>-1</option>
            <option value={0}>0</option>
            <option value={1}>+1</option>
            <option value={2}>+2</option>
          </select>
          <span className="value">{settings.octaveOffset >= 0 ? `+${settings.octaveOffset}` : settings.octaveOffset}</span>
        </div>
      </div>
    </div>
  );
};
