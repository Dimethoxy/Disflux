//==============================================================================

#pragma once

#include "../dsp/filter/FilterProcessor.h"

#include "OscillatorParameters.h"

#include <JuceHeader.h>

//==============================================================================
namespace dmt {
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
  return juce::AudioProcessorValueTreeState::ParameterLayout{
    std::make_unique<ParameterFloat>("oscGain",                // parameter ID
                                     "Attack",                 // parameter name
                                     NormalisableRange(0.0f,   // rangeStart
                                                       1.0f,   // rangeEnd
                                                       0.001f, // intervalValue
                                                       0.5f),  // skewFactor
                                     0.0f),                    // defaultValue
    std::make_unique<ParameterGroup>(Model::oscillatorParameterGroup(1))
  };
}
} // namespace dmt
