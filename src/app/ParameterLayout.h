#pragma once
//==============================================================================
#include "DmtHeader.h"
#include <JuceHeader.h>
//==============================================================================
static inline juce::AudioProcessorValueTreeState::ParameterLayout
createParameterLayout()
{
  using ParameterGroup = juce::AudioProcessorParameterGroup;
  namespace Model = dmt::model;

  juce::String uid = "";

  // Version hints for safe parameter addition in AudioUnit hosts
  constexpr int versionHint = 10100; // 01 01 00 = 1.1.0

  return juce::AudioProcessorValueTreeState::ParameterLayout{
    std::make_unique<ParameterGroup>(Model::globalParameterGroup()),
    std::make_unique<ParameterGroup>(
      Model::disfluxParameterGroup(uid, versionHint)),
  };
}
