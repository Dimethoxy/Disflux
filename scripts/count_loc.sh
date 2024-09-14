#!/bin/bash

directoryPath="./"
totalLoc=0
declare -A locPerLanguage

# Display ignored folders
echo "------------------------------------------------------------"
echo "Ignored folders: build, external (except dmt)"
echo "------------------------------------------------------------"

files=$(find "$directoryPath" -type f \( -name "*.h" -o -name "*.cpp" -o -name "CMakeLists.txt" -o -name "*.ps1" -o -name "*.sh" \) ! -path "*/build/*" \( ! -path "*/external/*" -o -path "*/external/dmt/*" \))

for file in $files; do
    extension="${file##*.}"
    loc=$(wc -l < "$file")
    echo "File: $file, LOC: $loc"

    # Update lines count for the specific language
    if [[ -n "${locPerLanguage[$extension]}" ]]; then
        locPerLanguage[$extension]=$((locPerLanguage[$extension] + loc))
    else
        locPerLanguage[$extension]=$loc
    fi

    totalLoc=$((totalLoc + loc))
done

echo "------------------------------------------------------------"

# Display lines per language
for language in "${!locPerLanguage[@]}"; do
    echo "Total LOC for $language : ${locPerLanguage[$language]}"
done
echo "------------------------------------------------------------"
echo "Combined LOC in folder: $totalLoc"
