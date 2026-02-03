#pragma once

#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace model {
static inline juce::AudioProcessorParameterGroup
globalParameterGroup(int versionHint)
{
  using ParameterBool = juce::AudioParameterBool;
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;

  juce::String uid = "Global";

  return juce::AudioProcessorParameterGroup(
    uid,      // group ID
    "Global", // group name
    "|",      // separator
    std::make_unique<ParameterFloat>(
      juce::ParameterID(uid + "Gain", versionHint),
      "Gain",
      NormalisableRange(-100.f, 100.f, 0.01f, 1.f),
      0,
      juce::String()),
    std::make_unique<ParameterBool>(
      juce::ParameterID(uid + "Bypass", versionHint),
      "Bypass",
      false,
      juce::String()));
}
} // namespace model
} // namespace dmt