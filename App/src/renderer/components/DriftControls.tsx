import React from 'react';
import { Knob } from './controls/Knob';
import './DriftControls.css';

export interface DriftSettings {
  rate: number;
  amount: number;
  jitter: number;
}

interface DriftControlsProps {
  settings: DriftSettings;
  onChange: (settings: DriftSettings) => void;
}

export const DriftControls: React.FC<DriftControlsProps> = ({ settings, onChange }) => {
  const handleChange = (partial: Partial<DriftSettings>) => {
    onChange({ ...settings, ...partial });
  };

  return (
    <div className="drift-panel">
      <div className="drift-panel__header">
        <span className="drift-panel__title">Oscillator Drift</span>
        <span className="drift-panel__subtitle">Analog feel</span>
      </div>
      <div className="drift-panel__knobs">
        <Knob
          label="Rate"
          min={0.05}
          max={5}
          step={0.01}
          value={settings.rate}
          onChange={(value) => handleChange({ rate: value })}
          formatValue={(value) => value.toFixed(2)}
          unit="Hz"
        />
        <Knob
          label="Drift"
          min={0}
          max={20}
          step={0.1}
          value={settings.amount}
          onChange={(value) => handleChange({ amount: value })}
          formatValue={(value) => value.toFixed(1)}
          unit="¢"
        />
        <Knob
          label="Jitter"
          min={0}
          max={15}
          step={0.1}
          value={settings.jitter}
          onChange={(value) => handleChange({ jitter: value })}
          formatValue={(value) => value.toFixed(1)}
          unit="¢"
        />
      </div>
    </div>
  );
};
