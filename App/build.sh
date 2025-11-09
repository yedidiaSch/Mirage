#!/bin/bash
# SynthUI Desktop - Full Build Script

set -e  # Exit on error

echo "================================"
echo "  SynthUI Desktop - Full Build"
echo "================================"
echo ""

# Navigate to App directory
cd "$(dirname "$0")"

echo "🧹 Cleaning old build..."
rm -rf build dist
echo "✓ Clean complete"
echo ""

echo "🔨 Building native C++ module..."
npm run build:native
echo "✓ Native module built"
echo ""

echo "📦 Building TypeScript (Main Process)..."
npm run build:main
echo "✓ Main process built"
echo ""

echo "📦 Building React (Renderer Process)..."
npm run build:renderer
echo "✓ Renderer built"
echo ""

echo "📋 Copying native module to dist..."
mkdir -p dist
cp build/Release/audioSystemNative.node dist/
echo "✓ Native module copied"
echo ""

echo "================================"
echo "✅ Build complete!"
echo "================================"
echo ""
echo "To run the app, use: ./run.sh"
echo "Or: npm start"
echo ""
