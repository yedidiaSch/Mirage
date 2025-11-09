import React, { useEffect, useState } from 'react';
import './MidiStatus.css';

interface MidiStatusProps {
  audioService: any;
}

export const MidiStatus: React.FC<MidiStatusProps> = ({ audioService }) => {
  const [midiStatus, setMidiStatus] = useState<{ connected: boolean; deviceName: string }>({
    connected: false,
    deviceName: ''
  });

  useEffect(() => {
    const checkMidiStatus = () => {
      try {
        if (audioService?.initialized) {
          const status = audioService.getMidiStatus();
          setMidiStatus(status);
        }
      } catch (error) {
        console.error('Failed to get MIDI status:', error);
      }
    };

    // Check immediately
    checkMidiStatus();

    // Check periodically (in case device is connected later)
    const interval = setInterval(checkMidiStatus, 2000);

    return () => clearInterval(interval);
  }, [audioService]);

  return (
    <div className={`midi-status ${midiStatus.connected ? 'connected' : 'disconnected'}`}>
      <div className="midi-status__indicator">
        <span className="midi-status__icon">🎹</span>
        <span className="midi-status__label">
          {midiStatus.connected ? 'MIDI' : 'No MIDI'}
        </span>
      </div>
      {midiStatus.connected && midiStatus.deviceName && (
        <div className="midi-status__device-name" title={midiStatus.deviceName}>
          {midiStatus.deviceName}
        </div>
      )}
    </div>
  );
};
