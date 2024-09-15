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
INFO_EMOJI="📋"
WARNING_EMOJI="⚠️"
FILE_EMOJI="📄"
LANGUAGE_EMOJI="💻"
SUMMARY_EMOJI="📝"

directoryPath="./"
totalLoc=0
declare -A locPerLanguage

echo -e "${CYAN}+-------------------------------------------------+${NC}"

# Display ignored folders
echo -e "${WARNING_EMOJI} ${YELLOW}Ignored folders: build, external (except dmt)${NC}"
echo -e "${CYAN}+-------------------------------------------------+${NC}"

files=$(find "$directoryPath" -type f \( -name "*.h" -o -name "*.cpp" -o -name "CMakeLists.txt" -o -name "*.ps1" -o -name "*.sh" \) ! -path "*/build/*" \( ! -path "*/external/*" -o -path "*/external/dmt/*" \))

for file in $files; do
    extension="${file##*.}"
    loc=$(wc -l < "$file")
    echo -e "${FILE_EMOJI} ${BLUE}File:${NC} $file, ${LANGUAGE_EMOJI} ${GREEN}LOC:${NC} $loc"

    # Update lines count for the specific language
    if [[ -n "${locPerLanguage[$extension]}" ]]; then
        locPerLanguage[$extension]=$((locPerLanguage[$extension] + loc))
    else
        locPerLanguage[$extension]=$loc
    fi

    totalLoc=$((totalLoc + loc))
done

echo -e "${CYAN}+-------------------------------------------------+${NC}"

# Display lines per language
for language in "${!locPerLanguage[@]}"; do
    echo -e "${LANGUAGE_EMOJI} ${MAGENTA}Total LOC for $language : ${NC}${locPerLanguage[$language]}"
done

echo -e "${CYAN}+-------------------------------------------------+${NC}"
echo -e "${SUMMARY_EMOJI} ${GREEN}Combined LOC in folder: ${NC}$totalLoc"
