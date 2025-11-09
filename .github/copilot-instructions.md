## Project Overview
- Electron + React front end (`App/src/renderer`) drives a native audio engine implemented in C++ (`audioSystem`).
- Native bridge is built with node-gyp; the compiled module `audioSystemNative.node` is required at runtime.
- Renderer implements a vintage-styled synthesizer UI with waveform oscilloscope, dual oscillator controls, and MIDI status.

## Build & Run
- `npm run build:native` — build the C++ addon; copies the resulting `.node` file into `App/dist` via the postbuild hook.
- `npm run build:main` — bundle Electron main-process code with Webpack.
- `npm run build:renderer` — bundle React renderer with Webpack.
- `npm run build` — run all three steps sequentially.
- `npm start` — launch the Electron app once all bundles are in `App/dist`.

## Coding Notes
- TypeScript/React code lives under `App/src/renderer`; prefer functional components and hooks.
- Styling is plain CSS modules per component; honor the warm vintage palette already in use.
- Native additions must be registered through `App/native/AudioSystemWrapper.{h,cpp}` and declared in `App/src/types/native.d.ts`.
- Keep `binding.gyp` in sync with native sources; avoid referencing files that are not compiled.

## Visualization Controls
- `WaveformDisplay` fetch cadence is governed by props `refreshMs` and `maxFrames`; downsampling logic lives inside `WaveformDisplay.tsx`.
- The oscilloscope currently requests up to 4096 frames (~93 ms) every 120 ms and displays the most recent slice.

## Audio Features
- Secondary oscillator configuration: mix, detune (cents), octave offset; all routed through `AudioService.configureSecondaryOscillator`.
- Pitch wheel support expects raw MIDI values (-8192..8191); converted to ±0.5 semitone in `AudioSystem::setPitchBend`.

## Platform Considerations
- Linux is the primary target: relies on system `rtaudio`, `rtmidi`, `libxml2`.

## Testing Checklist
- After native changes: `npm run build:native`.
- After renderer changes: `npm run build:renderer` and `npm start`.
- Watch for missing `.node` artifacts; copy resides in `App/dist/`.
