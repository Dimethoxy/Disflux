#pragma once
//==============================================================================
#include <JuceHeader.h>
//==============================================================================
namespace dmt {
namespace model {
//==============================================================================
static inline juce::AudioProcessorParameterGroup
disfluxParameterGroup(juce::String parentUid, int versionHint)
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  // using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;

  juce::String uid = parentUid + "Disflux";

  return juce::AudioProcessorParameterGroup(
    uid,       // group ID
    "Disflux", // group name
    "|",       // separator
    std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID(uid + "Amount", versionHint),
      "Amount",
      NormalisableRange(0.0f,   // rangeStart
                        256.0f, // rangeEnd
                        1.0f,   // intervalValue
                        0.4f),  // skewFactor
      1.0f,                     // defaultValue
      juce::String()),          // label
    std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID(uid + "Spread", versionHint),
      "Spread",
      NormalisableRange(0.0f,     // rangeStart
                        10000.0f, // rangeEnd
                        1.0f,     // intervalValue
                        0.5f),    // skewFactor
      200.0f,                     // defaultValue
      juce::String()),
    std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID(uid + "Frequency", versionHint),
      "Frequency",
      NormalisableRange(20.0f,    // rangeStart
                        20000.0f, // rangeEnd
                        1.0f,     // intervalValue
                        0.25f),   // skewFactor
      50.0f,                      // defaultValue
      juce::String()),
    std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID(uid + "Pinch", versionHint),
      "Pinch",
      NormalisableRange(0.5f,   // rangeStart
                        16.0f,  // rangeEnd
                        0.001f, // intervalValue
                        1.0f),  // skewFactor
      1.0f,                     // defaultValue
      juce::String()),
    std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID(uid + "Mix", versionHint),
      "Mix",
      NormalisableRange(0.0f,  // rangeStart
                        1.0f,  // rangeEnd
                        0.01f, // intervalValue
                        1.0f), // skewFactor
      1.0f,                    // defaultValue
      juce::String()));
}
} // namespace model
} // namespace dmt