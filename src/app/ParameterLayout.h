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

  return juce::AudioProcessorValueTreeState::ParameterLayout{
    std::make_unique<ParameterGroup>(Model::globalParameterGroup()),
    std::make_unique<ParameterGroup>(Model::disfluxParameterGroup(uid)),
  };
}
