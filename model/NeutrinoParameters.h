#pragma once
//==============================================================================
#include <JuceHeader.h>
//==============================================================================
namespace dmt {
namespace model {
//==============================================================================
static inline juce::AudioProcessorParameterGroup
neutrinoParameterGroup(juce::String parentUid, int versionHint)
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  // using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;

  juce::String uid = parentUid + "Neutrino";

  return juce::AudioProcessorParameterGroup(
    uid,        // group ID
    "Neutrino", // group name
    "|",        // separator

    // Symmetry
    std::make_unique<ParameterFloat>(uid + "Symmetry",
                                     "Symmetry",
                                     NormalisableRange(-1.f, // rangeStart
                                                       1.f,  // rangeEnd
                                                       .01f, // intervalValue
                                                       1.f), // skewFactor
                                     .0f));
}
} // namespace model
} // namespace dmt