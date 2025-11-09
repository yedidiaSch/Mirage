import React from 'react';
import './EnvelopeControls.css';

/**
 * EnvelopeControls component
 * Follows Single Responsibility Principle - only handles ADSR envelope controls
 */
interface EnvelopeControlsProps {
  adsr: {
    attack: number;
    decay: number;
    sustain: number;
    release: number;
  };
  onEnvelopeChange: (adsr: EnvelopeControlsProps['adsr']) => void;
}

export const EnvelopeControls: React.FC<EnvelopeControlsProps> = ({
  adsr,
  onEnvelopeChange
}) => {
  const handleChange = (param: keyof typeof adsr, value: number) => {
    onEnvelopeChange({
      ...adsr,
      [param]: value
    });
  };

  return (
    <div className="envelope-controls">
      <h3>ADSR Envelope</h3>
      <div className="envelope-sliders">
        <div className="slider-group">
          <label htmlFor="attack">Attack: {adsr.attack.toFixed(3)}s</label>
          <input
            id="attack"
            type="range"
            min="0.001"
            max="2"
            step="0.001"
            value={adsr.attack}
            onChange={(e) => handleChange('attack', parseFloat(e.target.value))}
          />
        </div>
        <div className="slider-group">
          <label htmlFor="decay">Decay: {adsr.decay.toFixed(3)}s</label>
          <input
            id="decay"
            type="range"
            min="0.001"
            max="2"
            step="0.001"
            value={adsr.decay}
            onChange={(e) => handleChange('decay', parseFloat(e.target.value))}
          />
        </div>
        <div className="slider-group">
          <label htmlFor="sustain">Sustain: {adsr.sustain.toFixed(2)}</label>
          <input
            id="sustain"
            type="range"
            min="0"
            max="1"
            step="0.01"
            value={adsr.sustain}
            onChange={(e) => handleChange('sustain', parseFloat(e.target.value))}
          />
        </div>
        <div className="slider-group">
          <label htmlFor="release">Release: {adsr.release.toFixed(3)}s</label>
          <input
            id="release"
            type="range"
            min="0.001"
            max="3"
            step="0.001"
            value={adsr.release}
            onChange={(e) => handleChange('release', parseFloat(e.target.value))}
          />
        </div>
      </div>
    </div>
  );
};
