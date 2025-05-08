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

  // Generate version hint from JUCE plugin version info
  constexpr int versionHint = 1;

  return juce::AudioProcessorValueTreeState::ParameterLayout{
    std::make_unique<ParameterGroup>(Model::globalParameterGroup(versionHint)),
    std::make_unique<ParameterGroup>(
      Model::disfluxParameterGroup(uid, versionHint)),
  };
}
