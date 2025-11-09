#!/bin/bash

# AudioSystem CMake Build Script
# This script builds both console and GUI applications using CMake

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔧 AudioSystem CMake Build Script${NC}"

# Default values
BUILD_TYPE="Release"
BUILD_GUI="ON"
BUILD_DIR="build"
CLEAN_BUILD=false
INSTALL_DEPS=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --no-gui)
            BUILD_GUI="OFF"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --install-deps)
            INSTALL_DEPS=true
            shift
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "OPTIONS:"
            echo "  --debug         Build in debug mode (default: release)"
            echo "  --release       Build in release mode"
            echo "  --no-gui        Skip GUI application build"
            echo "  --clean         Clean build directory before building"
            echo "  --install-deps  Install system dependencies"
            echo "  --build-dir DIR Use custom build directory (default: build)"
            echo "  --help, -h      Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}❌ Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

# Install dependencies if requested
if [ "$INSTALL_DEPS" = true ]; then
    echo -e "${YELLOW}📦 Installing system dependencies...${NC}"
    
    # Check if running on Ubuntu/Debian
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            pkg-config \
            libasound2-dev \
            librtmidi-dev \
            libxml2-dev \
            libglfw3-dev \
            libgl1-mesa-dev \
            libx11-dev \
            libxrandr-dev \
            libxinerama-dev \
            libxcursor-dev
    else
        echo -e "${YELLOW}⚠️  Please install dependencies manually for your system${NC}"
    fi
fi

# Initialize and update submodules
echo -e "${YELLOW}🔄 Updating Git submodules...${NC}"
git submodule update --init --recursive

# Build GUI submodule if needed
if [ "$BUILD_GUI" = "ON" ] && [ -d "guiBase_cpp" ]; then
    echo -e "${YELLOW}🏗️  Building GUI submodule...${NC}"
    cd guiBase_cpp
    if [ ! -d "build" ] || [ "$CLEAN_BUILD" = true ]; then
        rm -rf build
        mkdir -p build
    fi
    cd build
    cmake ..
    make -j$(nproc)
    cd ../..
fi

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}🧹 Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo -e "${YELLOW}⚙️  Configuring CMake...${NC}"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DBUILD_GUI="$BUILD_GUI" \
      ..

# Build
echo -e "${YELLOW}🔨 Building AudioSystem...${NC}"
make -j$(nproc)

echo -e "${GREEN}✅ Build completed successfully!${NC}"
echo ""
echo -e "${BLUE}📁 Build outputs:${NC}"
ls -la bin/

echo ""
echo -e "${BLUE}🚀 To run applications:${NC}"
echo -e "  Console app: ${GREEN}./$BUILD_DIR/bin/audioApp${NC}"
if [ "$BUILD_GUI" = "ON" ] && [ -f "bin/audioGUI" ]; then
    echo -e "  GUI app:     ${GREEN}./$BUILD_DIR/bin/audioGUI${NC}"
fi

echo ""
echo -e "${BLUE}🔧 To install system-wide:${NC}"
echo -e "  ${GREEN}sudo make install${NC} (from $BUILD_DIR directory)"