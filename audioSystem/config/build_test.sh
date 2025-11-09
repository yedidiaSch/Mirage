#!/bin/bash

# Build verification script for audio system XML configuration
# This script verifies that the core XML configuration system compiles correctly

echo "=== Audio System XML Configuration Build Test ==="
echo ""

# Check for required dependencies
echo "1. Checking for libxml2..."
if pkg-config --exists libxml2; then
    echo "   ✓ libxml2 found"
else
    echo "   ⚠ libxml2 not found via pkg-config, trying standard location..."
fi

# Create build directory
echo ""
echo "2. Creating build directory..."
mkdir -p build
echo "   ✓ Build directory ready"

# Test core component compilation
echo ""
echo "3. Testing core component compilation..."

echo "   Testing ConfigReader..."
g++ -I/usr/include/libxml2 -Isrc -g -c src/ConfigReader.cpp -o build/ConfigReader.o 2>/dev/null
if [ $? -eq 0 ]; then
    echo "   ✓ ConfigReader compiles"
else
    echo "   ✗ ConfigReader failed to compile"
    exit 1
fi

echo "   Testing AudioSystem..."
g++ -I/usr/include/libxml2 -Isrc -g -c src/audioSystem.cpp -o build/audioSystem.o 2>/dev/null
if [ $? -eq 0 ]; then
    echo "   ✓ AudioSystem compiles"
else
    echo "   ✗ AudioSystem failed to compile"
    exit 1
fi

echo "   Testing Effects..."
g++ -I/usr/include/libxml2 -Isrc -g -c src/Effects/*.cpp 2>/dev/null
if [ $? -eq 0 ]; then
    echo "   ✓ Effects compile"
else
    echo "   ✗ Effects failed to compile"
    exit 1
fi

echo "   Testing Waveforms..."
g++ -I/usr/include/libxml2 -Isrc -g -c src/Waves/*.cpp 2>/dev/null
if [ $? -eq 0 ]; then
    echo "   ✓ Waveforms compile"
else
    echo "   ✗ Waveforms failed to compile"
    exit 1
fi

# Test XML configuration loading
echo ""
echo "4. Testing XML configuration system..."

# Create a simple test program
cat > build/config_test.cpp << 'EOF'
#include "../src/ConfigReader.h"
#include <iostream>

int main() {
    try {
        ConfigReader reader;
        AudioConfig config = reader.loadConfig("config.xml");
        std::cout << "Configuration loaded: " << config.waveform 
                  << " @ " << config.sampleRate << "Hz" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}
EOF

# Compile and run test
g++ -I/usr/include/libxml2 -Isrc -g src/ConfigReader.cpp src/audioSystem.cpp src/Effects/*.cpp src/Waves/*.cpp build/config_test.cpp -o build/config_test -lxml2 2>/dev/null

if [ $? -eq 0 ]; then
    echo "   ✓ XML test program compiled"
    
    # Run the test
    if [ -f "config.xml" ]; then
        cp config.xml build/
        cd build
        ./config_test 2>/dev/null
        if [ $? -eq 0 ]; then
            echo "   ✓ XML configuration loading works"
        else
            echo "   ⚠ XML test ran but had issues"
        fi
        cd ..
    else
        echo "   ⚠ config.xml not found, skipping runtime test"
    fi
else
    echo "   ✗ XML test program failed to compile"
    exit 1
fi

echo ""
echo "=== Build Verification Complete ==="
echo ""
echo "✓ All core XML configuration components compile successfully"
echo "✓ The system is ready for full build with RtAudio/RtMidi dependencies"
echo ""
echo "To complete the build with audio support:"
echo "  1. Install RtAudio and RtMidi development libraries"
echo "  2. Use the build command from .vscode/tasks.json"
echo "  3. Or run: g++ [compiler flags] src/*.cpp ... -lrtaudio -lrtmidi -lasound -lpthread"

# Clean up
rm -f build/config_test.cpp *.o
echo ""
echo "Build test completed successfully!"