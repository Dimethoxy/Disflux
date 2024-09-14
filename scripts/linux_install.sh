#!/bin/bash

CLAP="build/src/OscilloscopePlugin_artefacts/Release/CLAP/Oscilloscope.clap"
VST="build/src/OscilloscopePlugin_artefacts/Release/VST3/Oscilloscope.vst3"
LV2="build/src/OscilloscopePlugin_artefacts/Release/LV2/Oscilloscope.lv2"

mkdir -p ~/.clap ~/.vst3 ~/.lv2
cp -r $CLAP ~/.clap
cp -r $VST ~/.vst3
cp -r $LV2 ~/.lv2