#pragma once
//==============================================================================
#include "DmtHeader.h"
#include <JuceHeader.h>
//==============================================================================
static inline juce::AudioProcessorValueTreeState::ParameterLayout
createParameterLayout()
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  using ParameterChoice = juce::AudioParameterChoice;
  using ParameterGroup = juce::AudioProcessorParameterGroup;
  using NormalisableRange = juce::NormalisableRange<float>;
  namespace Model = dmt::model;

  juce::String uid = "";

  return juce::AudioProcessorValueTreeState::ParameterLayout{
    std::make_unique<ParameterGroup>(Model::disfluxParameterGroup(uid))
  };
}
