# SynthUI Desktop Application

A desktop synthesizer application built with Electron, React, and C++ audio engine using N-API.

## Architecture

This application follows SOLID principles with a clean separation of concerns:

### Project Structure

```
App/
├── native/                    # N-API C++ bindings
│   ├── AudioSystemWrapper.h   # Header for N-API wrapper
│   └── AudioSystemWrapper.cpp # N-API implementation
├── src/
│   ├── main/                  # Electron main process
│   │   ├── main.ts           # Application entry point
│   │   └── preload.ts        # Preload script
│   ├── renderer/              # React UI
│   │   ├── components/       # UI components (SOLID)
│   │   │   ├── Synthesizer.tsx
│   │   │   ├── WaveformSelector.tsx
│   │   │   ├── EnvelopeControls.tsx
│   │   │   └── Keyboard.tsx
│   │   ├── App.tsx
│   │   └── index.tsx
│   ├── services/              # Business logic layer
│   │   └── AudioService.ts   # Audio system service
│   ├── utils/                 # Utility functions
│   │   └── NoteCalculator.ts # Note frequency calculations
│   └── types/                 # TypeScript type definitions
│       ├── native.d.ts       # Native module types
│       └── index.ts          # Application types
├── binding.gyp                # Node-gyp build configuration
├── package.json
├── tsconfig.json
└── webpack.*.config.js        # Webpack configurations
```

## Design Principles

### SOLID Principles Applied

1. **Single Responsibility Principle (SRP)**
   - Each component has one reason to change
   - `AudioService`: Manages audio system lifecycle
   - `WaveformSelector`: Handles waveform selection UI
   - `EnvelopeControls`: Manages ADSR parameters
   - `Keyboard`: Handles keyboard UI and note triggering
   - `NoteCalculator`: Calculates note frequencies

2. **Open/Closed Principle (OCP)**
   - Components are open for extension, closed for modification
   - New waveforms can be added without changing existing components
   - Effects can be extended in the C++ layer

3. **Liskov Substitution Principle (LSP)**
   - TypeScript interfaces ensure components can be substituted
   - Native bindings implement well-defined interfaces

4. **Interface Segregation Principle (ISP)**
   - Components receive only the props they need
   - No fat interfaces forcing unnecessary dependencies

5. **Dependency Inversion Principle (DIP)**
   - High-level modules depend on abstractions (TypeScript interfaces)
   - Services and components interact through interfaces

## Building the Application

### Prerequisites

- Node.js (v18+)
- npm or yarn
- C++ compiler (gcc/g++)
- CMake (for C++ audio system)
- RtAudio and RtMidi libraries

### Installation

```bash
cd App
npm install
```

### Build Steps

1. **Build the native addon:**
```bash
npm run build:native
```

2. **Build the Electron app:**
```bash
npm run build
```

3. **Run the application:**
```bash
npm start
```

### Development Mode

```bash
npm run dev
```

This starts webpack in watch mode and launches Electron with hot reload.

## Components Overview

### AudioSystemWrapper (C++)
- Exposes C++ AudioSystem to JavaScript via N-API
- Handles memory management and type conversion
- Provides methods: triggerNote, triggerNoteOff, setWaveform, updateADSRParameters

### AudioService (TypeScript)
- JavaScript layer wrapping the native module
- Provides initialization and error handling
- Clean API for React components

### React Components
- **Synthesizer**: Main orchestrator component
- **WaveformSelector**: Waveform type selection (sine, square, saw, triangle)
- **EnvelopeControls**: ADSR envelope parameter controls
- **Keyboard**: Virtual keyboard for note input

## Technologies

- **Electron**: Desktop application framework
- **React**: UI framework
- **TypeScript**: Type safety
- **N-API**: Native C++ bindings
- **Webpack**: Module bundler
- **C++ AudioSystem**: Core audio processing engine

## License

See LICENSE file in the audioSystem directory.
