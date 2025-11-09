import React from 'react';
import './WaveformSelector.css';

/**
 * WaveformSelector component
 * Follows Single Responsibility Principle - only handles waveform selection
 */
interface WaveformSelectorProps {
  selectedWaveform: 'sine' | 'square' | 'saw' | 'triangle';
  onWaveformChange: (waveform: 'sine' | 'square' | 'saw' | 'triangle') => void;
}

export const WaveformSelector: React.FC<WaveformSelectorProps> = ({
  selectedWaveform,
  onWaveformChange
}) => {
  const waveforms: Array<{ value: 'sine' | 'square' | 'saw' | 'triangle'; label: string }> = [
    { value: 'sine', label: 'Sine' },
    { value: 'square', label: 'Square' },
    { value: 'saw', label: 'Saw' },
    { value: 'triangle', label: 'Triangle' }
  ];

  return (
    <div className="waveform-selector">
      <h3>Waveform</h3>
      <div className="waveform-buttons">
        {waveforms.map(({ value, label }) => (
          <button
            key={value}
            className={`waveform-button ${selectedWaveform === value ? 'active' : ''}`}
            onClick={() => onWaveformChange(value)}
          >
            {label}
          </button>
        ))}
      </div>
    </div>
  );
};
