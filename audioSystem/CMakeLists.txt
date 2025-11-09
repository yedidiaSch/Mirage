cmake_minimum_required(VERSION 3.16)
project(AudioSystem VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add cmake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")

# Find required packages
find_package(PkgConfig QUIET)

# Find RtAudio - try pkg-config first, then fallback to custom finder
if(PkgConfig_FOUND)
    pkg_check_modules(RTAUDIO rtaudio)
endif()

if(NOT RTAUDIO_FOUND)
    find_package(RtAudio REQUIRED)
endif()

# Find RtMidi - try pkg-config first, then fallback to custom finder  
if(PkgConfig_FOUND)
    pkg_check_modules(RTMIDI rtmidi)
endif()

if(NOT RTMIDI_FOUND)
    find_package(RtMidi REQUIRED)
endif()

# Find LibXML2
find_package(LibXml2 REQUIRED)

# Find ALSA
if(PkgConfig_FOUND)
    pkg_check_modules(ALSA REQUIRED alsa)
else()
    find_library(ALSA_LIBRARIES NAMES asound)
    if(NOT ALSA_LIBRARIES)
        message(FATAL_ERROR "ALSA library not found")
    endif()
endif()

# Find Threads
find_package(Threads REQUIRED)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Core
    ${CMAKE_SOURCE_DIR}/src/Config
    ${CMAKE_SOURCE_DIR}/src/Common
    ${CMAKE_SOURCE_DIR}/src/Adapters
    ${CMAKE_SOURCE_DIR}/src/Applications
    ${CMAKE_SOURCE_DIR}/utilities
    ${RTAUDIO_INCLUDE_DIRS}
    ${RTMIDI_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(utilities)

# Create console application
add_executable(audioApp
    src/Applications/main.cpp
    $<TARGET_OBJECTS:audio_core>
    $<TARGET_OBJECTS:utilities_core>
)

target_link_libraries(audioApp
    ${RTAUDIO_LIBRARIES}
    ${RTMIDI_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${ALSA_LIBRARIES}
    Threads::Threads
)

target_compile_options(audioApp PRIVATE
    ${RTAUDIO_CFLAGS_OTHER}
    ${RTMIDI_CFLAGS_OTHER}
)

# GUI Application (optional)
option(BUILD_GUI "Build GUI application" ON)

if(BUILD_GUI)
    # Check if GUI submodule is available
    if(EXISTS "${CMAKE_SOURCE_DIR}/guiBase_cpp/CMakeLists.txt")
        # Find GUI dependencies
        find_package(glfw3 REQUIRED)
        find_package(OpenGL REQUIRED)
        find_package(X11 REQUIRED)

        # Add GUI submodule
        add_subdirectory(guiBase_cpp)

        # Create GUI application
        add_executable(audioGUI
            src/Applications/main_gui.cpp
            $<TARGET_OBJECTS:audio_core>
            $<TARGET_OBJECTS:gui_core>
            $<TARGET_OBJECTS:utilities_core>
        )

        target_include_directories(audioGUI PRIVATE
            ${CMAKE_SOURCE_DIR}/guiBase_cpp/include
            ${CMAKE_SOURCE_DIR}/src/GUI
        )

        target_link_libraries(audioGUI
            ${RTAUDIO_LIBRARIES}
            ${RTMIDI_LIBRARIES}
            ${LIBXML2_LIBRARIES}
            ${ALSA_LIBRARIES}
            Threads::Threads
            glfw
            OpenGL::GL
            ${X11_LIBRARIES}
            ${X11_Xrandr_LIB}
            ${X11_Xinerama_LIB}
            ${X11_Xcursor_LIB}
            ${CMAKE_DL_LIBS}
            guibase_static
        )

        target_compile_options(audioGUI PRIVATE
            ${RTAUDIO_CFLAGS_OTHER}
            ${RTMIDI_CFLAGS_OTHER}
        )
    else()
        message(WARNING "GUI submodule not found. GUI application will not be built. Run: git submodule update --init --recursive")
    endif()
endif()

# Install targets
install(TARGETS audioApp DESTINATION bin)

if(TARGET audioGUI)
    install(TARGETS audioGUI DESTINATION bin)
endif()

# Install configuration
install(DIRECTORY config/ DESTINATION share/audioSystem/config)

# Print configuration summary
message(STATUS "AudioSystem Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Console app: YES")
message(STATUS "  GUI app: ${BUILD_GUI}")
if(BUILD_GUI AND NOT TARGET audioGUI)
    message(STATUS "  GUI app available: NO (missing dependencies)")
endif()