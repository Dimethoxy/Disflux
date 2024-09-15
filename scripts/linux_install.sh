#!/bin/bash

# Define color codes
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
BLUE='\033[34m'
MAGENTA='\033[35m'
CYAN='\033[36m'
NC='\033[0m' # No Color

# Define emojis
SUCCESS_EMOJI="✅"
ERROR_EMOJI="❌"
INFO_EMOJI="📋"

echo "+-------------------------------------------------+"

# Check if the debug flag is passed
if [ "$1" == "--debug" ]; then
    echo -e "${INFO_EMOJI} ${CYAN}Using Debug build${NC}"
    CLAP="build/src/DisfluxPlugin_artefacts/Debug/CLAP/Disflux.clap"
    VST="build/src/DisfluxPlugin_artefacts/Debug/VST3/Disflux.vst3"
    LV2="build/src/DisfluxPlugin_artefacts/Debug/LV2/Disflux.lv2"
else
    echo -e "${INFO_EMOJI} ${CYAN}Using Release build${NC}"
    CLAP="build/src/DisfluxPlugin_artefacts/Release/CLAP/Disflux.clap"
    VST="build/src/DisfluxPlugin_artefacts/Release/VST3/Disflux.vst3"
    LV2="build/src/DisfluxPlugin_artefacts/Release/LV2/Disflux.lv2"
fi

# Create the necessary directories
mkdir -p ~/.clap ~/.vst3 ~/.lv2

# Track failures
FAILED=false

echo "+-------------------------------------------------+"
# Copy CLAP file
if cp -r "$CLAP" ~/.clap 2>/dev/null; then
    echo -e "${SUCCESS_EMOJI} ${GREEN}CLAP file copied successfully.${NC}"
else
    echo -e "${ERROR_EMOJI} ${RED}Error: Failed to copy CLAP file.${NC}"
    FAILED=true
fi

# Copy VST3 file
if cp -r "$VST" ~/.vst3 2>/dev/null; then
    echo -e "${SUCCESS_EMOJI} ${GREEN}VST3 file copied successfully.${NC}"
else
    echo -e "${ERROR_EMOJI} ${RED}Error: Failed to copy VST3 file.${NC}"
    FAILED=true
fi

# Copy LV2 file
if cp -r "$LV2" ~/.lv2 2>/dev/null; then
    echo -e "${SUCCESS_EMOJI} ${GREEN}LV2 file copied successfully.${NC}"
else
    echo -e "${ERROR_EMOJI} ${RED}Error: Failed to copy LV2 file.${NC}"
    FAILED=true
fi

echo "+-------------------------------------------------+"

# Final success/failure message
if [ "$FAILED" = false ]; then
    echo -e "${SUCCESS_EMOJI} ${GREEN}All files copied successfully.${NC}"
else
    echo -e "${ERROR_EMOJI} ${RED}One or more files failed to copy.${NC}"
fi
