# Audio Synthesis System

A real-time audio synthesis engine with both console and GUI interfaces, supporting MIDI input and built-in sequencer modes for generating digital audio with customizable effects cha### Future Improvements
- Add polyphony support (multiple notes at once)
- Implement advanced synthesis algorithms (FM, AM)
- Create more audio effects (reverb, chorus, distortion)
- **Enhance GUI with advanced features (effect parameter control, waveform visualization)**
- **Add configuration saving/loading through GUI interface**
- Support for audio file playback and recording
- Unit test framework and automated testing![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

## Features
- **Dual Interface Support**: Console application and interactive GUI
- Real-time audio processing using RtAudio
- MIDI input support via RtMidi
- **Built-in audio sequencer for testing without MIDI controller**
- **Interactive GUI for real-time parameter control**
- Modular effects chain system
- Configurable sample rate and buffer size
- Multiple waveforms including sine, square, sawtooth and triangle
- Built-in delay and low-pass filter effects
- Simple configuration API for selecting waves and effects
- **XML-based configuration with input mode selection**
- **Real-time audio parameter adjustment through GUI controls**

## Requirements
- C++14 compatible compiler
- RtAudio library
- RtMidi library
- ALSA development libraries (Linux)
- pthread library
- **For GUI application**: 
  - Dear ImGui (included via guiBase_cpp submodule)
  - GLFW library
  - OpenGL development libraries

## Setup

### CMake Build System (Recommended)
The project now uses CMake for cross-platform building. See [CMAKE_BUILD.md](CMAKE_BUILD.md) for detailed instructions.

**Quick Start:**
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake pkg-config libasound2-dev librtmidi-dev libxml2-dev libglfw3-dev libgl1-mesa-dev

# Initialize submodules and build
git submodule update --init --recursive
chmod +x build.sh
./build.sh

# Run applications
./build/bin/audioApp      # Console app
./build/bin/audioGUI      # GUI app
```

### Manual Installation of Dependencies
```bash
# Install ALSA development libraries
sudo apt-get install libasound2-dev

# Install RtMidi
sudo apt-get install librtmidi-dev

# Install GUI dependencies
sudo apt-get install libglfw3-dev libgl1-mesa-dev libxml2-dev

# Build and install RtAudio (if not available as package)
git clone https://github.com/thestk/rtaudio.git
cd rtaudio
mkdir build && cd build
cmake ..
make
sudo make install

# Initialize and build GUI submodule
git submodule update --init --recursive
cd guiBase_cpp
mkdir build && cd build
cmake ..
make
```

### Usage

#### Console Application
1. **MIDI Mode** (default): Connect a MIDI input device to your computer
2. **Sequencer Mode** (for testing): Edit `config/config.xml` and set `<mode>sequencer</mode>`
3. Build and run the console application:

```bash
# Build console app
make console
# or use VS Code task: "Build Console App"

# Run console app
./bin/audioApp
```

4. **MIDI Mode**: Play notes on your MIDI controller to generate audio
5. **Sequencer Mode**: Enjoy the automated musical sequence, press Enter to replay
6. Press Ctrl+C to stop the application

#### GUI Application
The GUI provides an interactive interface for real-time audio control and configuration:

```bash
# Build GUI app
make gui
# or use VS Code task: "Build GUI App"

# Run GUI app
./bin/audioGUI
```

**GUI Features:**
- **Real-time Waveform Selection**: Switch between sine, square, sawtooth, and triangle waves
- **Live Audio Parameter Control**: Adjust frequency, sample rate, and buffer size in real-time
- **Audio System Management**: Start/stop the audio system with dedicated controls
- **Interactive Sound Controls**: Play test tones and demo sequences with buttons
- **Visual Configuration**: All XML configuration parameters accessible through the interface
- **Input Mode Switching**: Toggle between MIDI and sequencer modes
- **Volume Control**: Real-time volume adjustment
- **Status Display**: Live monitoring of current settings and system status

**GUI Controls:**
- 🔊 **Start/Stop Audio System**: Initialize or shutdown the audio processing engine
- ▶️ **Play Test Tone**: Generate a 3-second test tone at current frequency
- 🎵 **Play Demo**: Execute a demo sequence with multiple notes
- **Waveform Selector**: Radio buttons for sine, square, sawtooth, triangle
- **Parameter Sliders**: Real-time adjustment of frequency, sample rate, buffer size
- **Input Mode**: Toggle between MIDI input and built-in sequencer

## Build Tasks

**Primary Build System:** CMake (see [CMAKE_BUILD.md](CMAKE_BUILD.md))

The project includes VS Code tasks for both CMake and legacy builds:

### CMake Tasks (Recommended)
- **CMake: Build All**: Builds both console and GUI applications
- **CMake: Build Console App**: Builds only the console application
- **CMake: Build GUI App**: Builds only the GUI application
- **Run Console App**: Builds and runs the console application
- **Run GUI App**: Builds and runs the GUI application

### Legacy Manual Build Commands (Deprecated)

#### Console Application
```bash
g++ -g -Wall -Wextra \
  src/main.cpp src/ConfigReader.cpp src/audioSystem.cpp src/audioDevice.cpp \
  src/AudioSequencer.cpp src/AudioSystemAdapter.cpp src/Midi/*.cpp \
  src/Effects/*.cpp src/Waves/*.cpp src/Envelope/*.cpp \
  utilities/QueueThread.cpp utilities/TimerFd.cpp utilities/subject.cpp utilities/threadBase.cpp \
  -I/usr/include/rtmidi -I/usr/include/rtaudio -I./src -I/usr/include/libxml2 \
  -lrtmidi -lrtaudio -lxml2 -lasound -lpthread \
  -o bin/audioApp
```

#### GUI Application
```bash
g++ -g -Wall -Wextra \
  src/main_gui.cpp src/ConfigReader.cpp src/audioSystem.cpp src/audioDevice.cpp \
  src/AudioSequencer.cpp src/AudioSystemAdapter.cpp src/Midi/*.cpp \
  src/Effects/*.cpp src/Waves/*.cpp src/Envelope/*.cpp \
  utilities/QueueThread.cpp utilities/TimerFd.cpp utilities/subject.cpp utilities/threadBase.cpp \
  -I./guiBase_cpp/include -I./src -I/usr/include/rtmidi -I/usr/include/rtaudio -I/usr/include/libxml2 \
  ./guiBase_cpp/build/lib/libguibase.a ./guiBase_cpp/build/libimgui.a \
  -lrtmidi -lrtaudio -lxml2 -lasound -lglfw -lGL -lX11 -lXrandr -lXinerama -lXcursor -ldl -lpthread \
  -o bin/audioGUI
```
The system supports two input modes configurable in `config/config.xml`:

**MIDI Mode:**
```xml
<input>
    <mode>midi</mode>
</input>
```

**Sequencer Mode (Testing):**
```xml
<input>
    <mode>sequencer</mode>
    <sequenceType>demo</sequenceType>  <!-- Options: scale, chord, melody, demo -->
</input>
```

### Configuration Example
The audio system can be configured at runtime with a simple structure:

```cpp
AudioConfig cfg;
cfg.waveform = "sawtooth";
cfg.effects = {"delay", "lowpass"};
audioSystem.configure(cfg);
```

This selects a sawtooth oscillator and adds delay and low-pass filter effects to the processing chain.
 

## Embedded FreeRTOS STM32 Example
See `embedded/` for a reference port using FreeRTOS and the STM32 HAL. It shows how to stream synthesized audio via DMA to I2S or DAC. The example builds a sine lookup table and allows custom waveform or effect callbacks via `AudioTaskInit`.

## Code Quality Features
- **Input Validation**: Comprehensive parameter validation throughout the codebase
- **Memory Safety**: Smart pointer usage and null pointer checks
- **Error Handling**: Graceful handling of invalid inputs and edge cases
- **Documentation**: Extensive Doxygen-style documentation for all interfaces
- **Consistent Coding Style**: Standardized formatting and naming conventions
- **Performance Optimizations**: Efficient phase calculations and reduced allocations

## Architecture
- **Modular Design**: Clean separation between waveforms, effects, and audio processing
- **Observer Pattern**: MIDI events are processed through a clean observer interface
- **Configurable Effects Chain**: Runtime configuration of waveform and effects
- **Case-Insensitive Configuration**: Flexible configuration with alias support

### Recent Improvements
- ✅ Complete ADSR envelope implementation
- ✅ Enhanced input validation and error handling
- ✅ Improved documentation and code consistency
- ✅ Performance optimizations in audio processing
- ✅ Better resource management and memory safety
- ✅ Case-insensitive configuration system
- ✅ **Built-in audio sequencer for testing without MIDI controller**
- ✅ **Configurable input modes (MIDI vs Sequencer)**
- ✅ **Multiple musical sequence types (scale, chord, melody, demo)**
- ✅ **Interactive GUI with real-time parameter control**
- ✅ **Live audio system management through GUI interface**
- ✅ **Visual waveform selection and audio parameter adjustment**

### Future Improvements
- Add polyphony support (multiple notes at once)
- Implement advanced synthesis algorithms (FM, AM)
- Create more audio effects (reverb, chorus, distortion)
- Add GUI for parameter control
- Support for audio file playback and recording
- Unit test framework and automated testing

### License
This project is available under the MIT License.

### Contributing
Contributions are welcome! Please feel free to submit a Pull Request. When contributing:
- Follow the existing code style and documentation standards
- Add appropriate input validation and error handling
- Include comprehensive documentation for new features
- Test your changes thoroughly before submitting









