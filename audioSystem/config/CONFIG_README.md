# Audio System XML Configuration

The audio system now supports XML-based configuration for all major parameters. This allows you to easily customize the sound characteristics and system behavior without recompiling the code.

## Configuration File

The main configuration file is `config.xml` in the root directory. If this file cannot be read, the system will fall back to sensible defaults.

### Configuration Sections

#### Audio Settings
```xml
<audio>
    <sampleRate>44100.0</sampleRate>    <!-- Sample rate in Hz -->
    <bufferFrames>512</bufferFrames>    <!-- Buffer size in frames -->
</audio>
```

- **sampleRate**: Audio sample rate in Hz. Common values:
  - 44100: CD quality (recommended for most use cases)
  - 48000: Professional audio standard
  - 22050: Lower quality, less CPU usage

- **bufferFrames**: Audio buffer size in frames. Affects latency and stability:
  - 256: Lower latency, higher CPU usage
  - 512: Balanced (recommended)
  - 1024: Higher latency, more stable
  - 2048: Very stable, high latency

#### Waveform Selection
```xml
<waveform>
    <type>sine</type>
</waveform>
```

Available waveform types:
- **sine**: Smooth, pure tone
- **square**: Sharp, digital sound
- **sawtooth** or **saw**: Bright, buzzy sound
- **triangle** or **tri**: Softer than sawtooth, warmer than sine

#### Effects Chain
```xml
<effects>
    <effect>delay</effect>
    <effect>lowpass</effect>
</effects>
```

Effects are applied in the order they appear. Available effects:
- **octave**: Adds higher octave harmonics
- **delay** or **echo**: Adds delayed repeats of the signal
- **lowpass**, **lpf**, or **filter**: Removes high frequencies for warmer sound

#### MIDI Configuration
```xml
<midi>
    <port>1</port>
</midi>
```

- **port**: MIDI input port number (0-based)
  - -1: Disable MIDI
  - 0: First available port
  - 1: Second available port (default)

#### Default Frequency
```xml
<defaultFrequency>440.0</defaultFrequency>
```

Default frequency for testing and initialization in Hz:
- 440.0: Standard A4 pitch
- 261.63: Middle C

#### Input Mode Configuration
```xml
<input>
    <mode>sequencer</mode>
    <sequenceType>demo</sequenceType>
</input>
```

- **mode**: Input mode for the audio system
  - **midi**: Use MIDI controller input (default)
  - **sequencer**: Use built-in audio sequencer for testing

- **sequenceType**: Type of musical sequence for sequencer mode
  - **demo**: Comprehensive demonstration sequence (default)
  - **scale**: Major scale (C-D-E-F-G-A-B-C)
  - **chord**: Chord progression arpeggios
  - **melody**: Simple melody ("Twinkle Twinkle Little Star")

## Example Configurations

### Default Configuration (`config.xml`)
Balanced setup with sine wave and basic effects.

### Vintage Sound (`config_vintage.xml`)
Warm, retro sound using triangle wave with lowpass filtering.

### Modern Sound (`config_modern.xml`)
Bright, contemporary sound using sawtooth wave with octave effects.

### Clean Sound (`config_clean.xml`)
Pure sine wave with no effects for testing.

### Sequencer Demo (`config_sequencer_demo.xml`)
Built-in musical sequences for testing without MIDI controller.

### MIDI Live Performance (`config_midi_live.xml`)
Optimized for low-latency live MIDI performance.

## Usage in Code

The configuration is loaded using the `ConfigReader` class:

```cpp
#include "ConfigReader.h"

// Load configuration
ConfigReader configReader;
AudioConfig config = configReader.loadConfig("config.xml");

// Initialize audio system with configuration
AudioSystem audioSystem = initializeAudioSystem(config);
AudioDevice audioDevice(&audioSystem, config.sampleRate, config.bufferFrames);
```

## Error Handling

If the XML file cannot be read or parsed:
- The system displays a warning message
- Default values are used automatically
- The program continues to run normally

This ensures the system is robust and will always start, even with a missing or corrupted configuration file.