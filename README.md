# SynthUI Workspace

SynthUI pairs an Electron/React front end with a native C++ audio engine to deliver a vintage-inspired synthesizer desktop app. This workspace contains both the desktop shell (`App/`) and the standalone audio engine project (`audioSystem/`).

## Repository Layout

- `App/` – Electron + React application, including the N-API bridge that loads the native audio module and the full UI.
- `audioSystem/` – C++ sources, effects, and utilities that implement the realtime synthesis engine. This project also builds CLI demos and tooling.
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

For rapid UI iteration you can run `npm run dev`, which starts the renderer bundler in watch mode and relaunches Electron on rebuild.

## Using the C++ Audio Engine Directly

From `audioSystem/` you can build and run the standalone applications:

```bash
./build.sh           # configure & build with CMake (outputs to audioSystem/build/)
./build/bin/audioApp # example headless synth runner
```

Additional demos live under `audioSystem/guiBase_cpp/` and `audioSystem/utilities/`.

## MIDI & Hardware Controls

When a hardware MIDI controller is connected the native layer routes events through `AudioSystemAdapter`. Control Change 7 remaps to the low-pass cutoff, and the React UI polls the current cutoff so hardware movement stays in sync with the on-screen knob.

## Licensing

The native engine ships with its own license inside `audioSystem/LICENSE`. If you intend to publish the combined application, ensure that license terms are compatible with your distribution.

## Contributing

1. Build the native module before touching the renderer so the `.node` artifact stays in sync.
2. Keep TypeScript and C++ changes aligned—update `App/src/types/native.d.ts` whenever the N-API surface changes.
3. Run `npm run build:native` and `npm run build:renderer` before posting pull requests to verify the code generation steps succeed.
