# SynthUI Workspace

SynthUI pairs an Electron/React front end with a native C++ audio engine to deliver a vintage-inspired synthesizer desktop app. The project lives in a single repository and is designed to build and run through the Electron shell—there is no separate, supported standalone build of the C++ engine.

## Repository Layout

- `App/` – Electron + React application, including the N-API bridge that loads the native audio module and the full UI.
- `audioSystem/` – C++ sources, effects, and utilities compiled into the native module consumed by the renderer.
- `.github/` – Copilot configuration and project-specific contributor guidance.

## Prerequisites

- Node.js 18+
- npm
- A C++17-capable toolchain (gcc/clang or MSVC)
- CMake 3.20+
- Platform audio dependencies (RtAudio, RtMidi, libxml2 on Linux)

## Building the Desktop App

```bash
cd App
npm install
npm run build:native   # builds the node-gyp addon (copies audioSystemNative.node into dist/)
npm run build          # bundles main + renderer processes
npm start              # launches Electron
```

### Development Workflow

- `npm run dev` – start the renderer bundler in watch mode and relaunch Electron on rebuild.
- `npm run build:renderer` – rebuild the React bundle without touching the native layer.
- `npm run build:main` – rebuild the Electron main process bundle.
- `npm run build:native` – recompile the audio engine when C++ sources change (copies `audioSystemNative.node` into `App/dist/`).
- `npm start` – launch the packaged Electron app from `App/dist/` once bundles are up to date.

## MIDI & Hardware Controls

When a hardware MIDI controller is connected the native layer routes events through `AudioSystemAdapter`. Control Change 7 remaps to the low-pass cutoff, and the React UI polls the current cutoff so hardware movement stays in sync with the on-screen knob.

## Licensing

The entire project is released under the MIT License. See the [LICENSE](./LICENSE) file for details.

## Contributing

1. Build the native module before touching the renderer so the `.node` artifact stays in sync.
2. Keep TypeScript and C++ changes aligned—update `App/src/types/native.d.ts` whenever the N-API surface changes.
3. Run `npm run build:native` and `npm run build:renderer` before posting pull requests to verify the code generation steps succeed.
