import React from 'react';
import './Knob.css';

interface KnobProps {
  label: string;
  value: number;
  min: number;
  max: number;
  step?: number;
  unit?: string;
  onChange: (value: number) => void;
  disabled?: boolean;
  formatValue?: (value: number) => string;
}

const clamp = (value: number, min: number, max: number) => {
  if (value < min) return min;
  if (value > max) return max;
  return value;
};

export const Knob: React.FC<KnobProps> = ({
  label,
  value,
  min,
  max,
  step = 0.01,
  unit = '',
  onChange,
  disabled = false,
  formatValue,
}) => {
  const clampedValue = clamp(value, min, max);
  const normalized = (clampedValue - min) / (max - min);
  const rotation = normalized * 270 - 135; // Map [0..1] to [-135..135]

  const displayValue = formatValue ? formatValue(clampedValue) : clampedValue.toFixed(2);

  return (
    <div className={`knob ${disabled ? 'knob--disabled' : ''}`}>
      <div className="knob__dial">
        <div className="knob__dial-inner" style={{ transform: `rotate(${rotation}deg)` }}>
          <span className="knob__pointer" />
        </div>
      </div>
      <input
        className="knob__input"
        type="range"
        min={min}
        max={max}
        step={step}
        value={clampedValue}
        onChange={(event) => onChange(parseFloat(event.target.value))}
        aria-label={label}
        aria-disabled={disabled}
      />
      <div className="knob__label">{label}</div>
      <div className="knob__value">
        {displayValue}
        {unit}
      </div>
    </div>
  );
};

export default Knob;
