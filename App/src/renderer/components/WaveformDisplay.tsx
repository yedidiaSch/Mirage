import React, { useEffect, useRef } from 'react';
import { AudioService } from '../../services/AudioService';
import './WaveformDisplay.css';

interface WaveformDisplayProps {
  audioService: AudioService;
  refreshMs?: number;
  maxFrames?: number;
}

/**
 * Canvas-based waveform visualizer that periodically samples the native audio ring buffer.
 * Keeps CPU usage low by drawing a decimated trace roughly every 100-200ms.
 */
export const WaveformDisplay: React.FC<WaveformDisplayProps> = ({
  audioService,
  refreshMs = 120,
  maxFrames = 2048
}) => {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const intervalRef = useRef<number | null>(null);
  const disposedRef = useRef(false);
  const emptyData = useRef<Float32Array>(new Float32Array(0));

  useEffect(() => {
    disposedRef.current = false;

    const ensureCanvasSize = (canvas: HTMLCanvasElement, ctx: CanvasRenderingContext2D) => {
      const rect = canvas.getBoundingClientRect();
      const dpr = window.devicePixelRatio || 1;
      const width = Math.max(1, Math.floor(rect.width));
      const height = Math.max(1, Math.floor(rect.height));
      const renderWidth = Math.max(1, Math.floor(width * dpr));
      const renderHeight = Math.max(1, Math.floor(height * dpr));

      if (canvas.width !== renderWidth || canvas.height !== renderHeight) {
        canvas.width = renderWidth;
        canvas.height = renderHeight;
        if (typeof ctx.resetTransform === 'function') {
          ctx.resetTransform();
        } else {
          ctx.setTransform(1, 0, 0, 1, 0, 0);
        }
        ctx.scale(dpr, dpr);
      }

      return { width, height };
    };

    const drawWaveform = (data: Float32Array) => {
      const canvas = canvasRef.current;
      if (!canvas) {
        return;
      }

      const ctx = canvas.getContext('2d');
      if (!ctx) {
        return;
      }

      const { width, height } = ensureCanvasSize(canvas, ctx);
      ctx.clearRect(0, 0, width, height);
      ctx.fillStyle = '#111';
      ctx.fillRect(0, 0, width, height);

      const frames = Math.floor(data.length / 2);
      const baseline = height / 2;
      if (frames === 0) {
        ctx.strokeStyle = '#333';
        ctx.beginPath();
        ctx.moveTo(0, baseline);
        ctx.lineTo(width, baseline);
        ctx.stroke();
        return;
      }

  const targetPoints = Math.max(1, Math.min(512, width));
      const framesPerPixel = 4;
      const minimumVisible = 128;
      const desiredFrames = Math.max(minimumVisible, targetPoints * framesPerPixel);
      const visibleFrames = Math.max(1, Math.min(frames, desiredFrames));
      const startFrame = Math.max(0, frames - visibleFrames);
      const span = Math.max(1, visibleFrames - 1);
      const step = Math.max(1, Math.floor(visibleFrames / targetPoints));
      const amplitude = height * 0.45;

      ctx.beginPath();
      ctx.strokeStyle = '#f3b74b';
      ctx.lineWidth = 1.5;

      for (let i = 0; i < visibleFrames; i += step) {
        const frame = startFrame + i;
        const sampleIndex = frame * 2;
        const mono = (data[sampleIndex] + data[sampleIndex + 1]) * 0.5;
        const x = (i / span) * width;
        const y = baseline - mono * amplitude;
        if (i === 0) {
          ctx.moveTo(x, y);
        } else {
          ctx.lineTo(x, y);
        }
      }

      ctx.stroke();
    };

    const tick = () => {
      if (disposedRef.current) {
        return;
      }

      const data = audioService.initialized
        ? audioService.getWaveformData(maxFrames)
        : emptyData.current;

      requestAnimationFrame(() => drawWaveform(data));
    };

    tick();
    intervalRef.current = window.setInterval(tick, refreshMs);

    return () => {
      disposedRef.current = true;
      if (intervalRef.current !== null) {
        window.clearInterval(intervalRef.current);
        intervalRef.current = null;
      }
    };
  }, [audioService, refreshMs, maxFrames]);

  return (
    <div className="waveform-container">
      <canvas ref={canvasRef} className="waveform-canvas" />
    </div>
  );
};
