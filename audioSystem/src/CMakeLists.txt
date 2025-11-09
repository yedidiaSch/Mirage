# Audio core library sources
set(AUDIO_CORE_SOURCES
    Config/ConfigReader.cpp
    Core/audioSystem.cpp
    Core/audioDevice.cpp
    Core/AudioSequencer.cpp
    Adapters/AudioSystemAdapter.cpp
    Midi/MidiDevice.cpp
    Effects/DelayEffect.cpp
    Effects/IEffect.cpp
    Effects/LowPassEffect.cpp
    Effects/OctaveEffect.cpp
    Waves/SineWave.cpp
    Waves/SquareWave.cpp
    Waves/SawtoothWave.cpp
    Waves/TriangleWave.cpp
    Envelope/ADSREnvelope.cpp
)

# GUI components sources (for clean architecture)
set(GUI_CORE_SOURCES
    GUI/AudioSystemManager.cpp
    GUI/ConfigurationManager.cpp
    GUI/SoundController.cpp
    GUI/GuiRenderers.cpp
    GUI/EffectParameterWindow.cpp
    GUI/EnvelopeControlRenderer.cpp
)

# Create object library for audio core
add_library(audio_core OBJECT ${AUDIO_CORE_SOURCES})

# Create object library for GUI components
add_library(gui_core OBJECT ${GUI_CORE_SOURCES})

target_include_directories(audio_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/Adapters
    ${CMAKE_CURRENT_SOURCE_DIR}/Applications
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/utilities
    ${RTAUDIO_INCLUDE_DIRS}
    ${RTMIDI_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
)

target_compile_options(audio_core PRIVATE
    ${RTAUDIO_CFLAGS_OTHER}
    ${RTMIDI_CFLAGS_OTHER}
)

# Configure GUI core library
target_include_directories(gui_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/Adapters
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/utilities
    ${CMAKE_SOURCE_DIR}/guiBase_cpp/include
    ${RTAUDIO_INCLUDE_DIRS}
    ${RTMIDI_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
)

target_compile_options(gui_core PRIVATE
    ${RTAUDIO_CFLAGS_OTHER}
    ${RTMIDI_CFLAGS_OTHER}
)