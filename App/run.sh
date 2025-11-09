#!/bin/bash
# SynthUI Desktop - Run Script

set -e  # Exit on error

echo "================================"
echo "  SynthUI Desktop Launcher"
echo "================================"
echo ""

# Navigate to App directory
cd "$(dirname "$0")"

# Check if build is needed
if [ ! -d "build/Release" ] || [ ! -f "build/Release/audioSystemNative.node" ]; then
    echo "⚙️  Native module not found. Building..."
    npm run build:native
else
    echo "✓ Native module found"
fi

if [ ! -d "dist" ] || [ ! -f "dist/main.js" ]; then
    echo "⚙️  Dist folder not complete. Building app..."
    npm run build:main
    npm run build:renderer
else
    echo "✓ App built"
fi

# Check if native module is copied to dist
if [ ! -f "dist/audioSystemNative.node" ]; then
    echo "⚙️  Copying native module to dist..."
    mkdir -p dist
    cp build/Release/audioSystemNative.node dist/
fi

echo ""
echo "🚀 Starting SynthUI Desktop..."
echo ""

# Run the app
npx --no-install electron . --no-sandbox
