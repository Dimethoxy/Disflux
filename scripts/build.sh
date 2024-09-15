#!/bin/bash

# Define color codes
RED='\033[31m'
GREEN='\033[32m'
CYAN='\033[36m'
MAGENTA='\033[35m'
YELLOW='\033[33m'
NC='\033[0m' # No Color

# Define emojis
SUCCESS_EMOJI="✅"
ERROR_EMOJI="❌"
INFO_EMOJI="📋"
CLEAN_EMOJI="🧹"
CONFIGURE_EMOJI="🔧"
BUILD_EMOJI="🛠️"

# Paths to install scripts (placeholders, update with actual paths)
INSTALL_SCRIPT_LINUX="./scripts/install_linux.sh"
INSTALL_SCRIPT_MAC="./scripts/install_mac.sh"
INSTALL_SCRIPT_WINDOWS="./scripts/install_windows.bat"

# Default settings
BUILD_DIR="build"
CMAKE_GENERATOR="Ninja"
BUILD_TYPE=""
OS_TYPE=""
PERFETTO_FLAG=""
INSTALL=false
CLEAN=false

# Display help
print_help() {
    echo -e "${CYAN}+-------------------------------------------------+${NC}"
    echo -e "${INFO_EMOJI} ${CYAN}Usage: bash build.sh [OPTIONS] [--os TYPE] [--build TYPE]${NC}"
    echo -e "${CYAN}+-------------------------------------------------+${NC}"
    echo -e "${INFO_EMOJI} ${CYAN}Options:${NC}"
    echo -e "  ${MAGENTA}--clean${NC}          Clean the build directory before building."
    echo -e "  ${MAGENTA}--install${NC}        Install the project after building."
    echo -e "  ${MAGENTA}--help${NC}           Show this help message."
    echo -e "  ${MAGENTA}--perfetto${NC}       Build with Perfetto flags."
    echo -e "  ${MAGENTA}--linux${NC}          Specify Linux as the OS type."
    echo -e "  ${MAGENTA}--mac${NC}            Specify macOS as the OS type."
    echo -e "  ${MAGENTA}--windows${NC}        Specify Windows as the OS type."
    echo -e "  ${MAGENTA}--debug${NC}          Specify Debug build type."
    echo -e "  ${MAGENTA}--release${NC}        Specify Release build type."
    echo -e "${CYAN}+-------------------------------------------------+${NC}"
}

# Parse arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --clean) CLEAN=true ;;
        --install) INSTALL=true ;;
        --help) print_help; exit 0 ;;
        --perfetto) PERFETTO_FLAG="-DPERFETTO=1" ;;
        --linux) OS_TYPE="Linux" ;;
        --mac) OS_TYPE="Darwin" ;;
        --windows) OS_TYPE="Windows" ;;
        --debug) BUILD_TYPE="Debug" ;;
        --release) BUILD_TYPE="Release" ;;
        *) echo -e "${ERROR_EMOJI} ${RED}Unknown parameter passed: $1${NC}" ; exit 1 ;;
    esac
    shift
done

# Validate required parameters
if [ -z "$OS_TYPE" ] || [ -z "$BUILD_TYPE" ]; then
    echo -e "${CYAN}+-------------------------------------------------+${NC}"
    echo -e "${ERROR_EMOJI} ${RED}Error: OS type and build type are required.${NC}"
    print_help
    exit 1
fi

# Display selected configuration
echo -e "${CYAN}+-------------------------------------------------+${NC}"
echo -e "${INFO_EMOJI} ${YELLOW}OS Type: ${NC}${OS_TYPE:-Not specified}"
echo -e "${INFO_EMOJI} ${YELLOW}Build Type: ${NC}${BUILD_TYPE:-Not specified}"
echo -e "${INFO_EMOJI} ${YELLOW}Perfetto Flag: ${NC}${PERFETTO_FLAG:-Not enabled}"
echo -e "${INFO_EMOJI} ${YELLOW}Install: ${NC}${INSTALL:-Not enabled}"
echo -e "${INFO_EMOJI} ${YELLOW}Clean: ${NC}${CLEAN:-Not enabled}"

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    echo -e "${CYAN}+-------------------------------------------------+${NC}"
    echo -e "${CLEAN_EMOJI} ${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
fi

# Configure and build
echo -e "${CYAN}+-------------------------------------------------+${NC}"
echo -e "${CONFIGURE_EMOJI} ${YELLOW}Configuring project...${NC}"

if ! cmake -G "$CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" $PERFETTO_FLAG .; then
    echo -e "${CYAN}+-------------------------------------------------+${NC}"
    echo -e "${ERROR_EMOJI} ${RED}Configuration failed.${NC}"
    exit 1
fi

echo -e "${CYAN}+-------------------------------------------------+${NC}"
echo -e "${BUILD_EMOJI} ${YELLOW}Building project...${NC}"

if ! cmake --build "$BUILD_DIR"; then
    echo -e "${CYAN}+-------------------------------------------------+${NC}"
    echo -e "${ERROR_EMOJI} ${RED}Build failed.${NC}"
    exit 1
fi

# Install if requested
if [ "$INSTALL" = true ]; then
    echo -e "${CYAN}+-------------------------------------------------+${NC}"
    echo -e "${INFO_EMOJI} ${YELLOW}Installing project...${NC}"
    case $OS_TYPE in
        Linux)
            bash "$INSTALL_SCRIPT_LINUX"
            ;;
        Darwin)
            bash "$INSTALL_SCRIPT_MAC"
            ;;
        Windows)
            cmd /c "$INSTALL_SCRIPT_WINDOWS"
            ;;
    esac
fi

echo -e "${CYAN}+-------------------------------------------------+${NC}"
echo -e "${SUCCESS_EMOJI} ${GREEN}Build process completed successfully.${NC}"
echo -e "${CYAN}+-------------------------------------------------+${NC}"
