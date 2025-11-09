#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

echo -e "${CYAN}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║${WHITE}                  UTILITIES TEST RUNNER                      ${CYAN}║${NC}"
echo -e "${CYAN}╚══════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Check if make is available
if ! command -v make &> /dev/null; then
    echo -e "${RED}❌ Make is not installed. Please install make to continue.${NC}"
    exit 1
fi

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}❌ g++ is not installed. Please install g++ to continue.${NC}"
    exit 1
fi

echo -e "${BLUE}🔨 Building test suite...${NC}"
echo ""

# Build the project
if make clean && make; then
    echo ""
    echo -e "${GREEN}✅ Build successful!${NC}"
    echo ""
    echo -e "${PURPLE}🧪 Running tests...${NC}"
    echo ""
    
    # Run the tests
    ./test_utilities
    
    # Capture the exit code
    exit_code=$?
    
    echo ""
    if [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}🎉 Test suite completed successfully!${NC}"
    else
        echo -e "${YELLOW}⚠️  Test suite completed with issues (exit code: $exit_code)${NC}"
    fi
    
    # Optional: Run with valgrind if requested
    if [[ "$1" == "--valgrind" ]] || [[ "$1" == "-v" ]]; then
        echo ""
        echo -e "${BLUE}🔍 Running memory leak detection...${NC}"
        echo ""
        make valgrind
    fi
    
    # Clean up build artifacts after testing
    echo ""
    echo -e "${BLUE}🧹 Cleaning up build artifacts...${NC}"
    make clean
    
else
    echo ""
    echo -e "${RED}❌ Build failed! Please check the compilation errors above.${NC}"
    exit 1
fi

echo ""
echo -e "${CYAN}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║${WHITE}                    TEST RUNNER COMPLETE                     ${CYAN}║${NC}"
echo -e "${CYAN}╚══════════════════════════════════════════════════════════════╝${NC}"