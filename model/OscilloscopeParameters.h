#pragma once
//==============================================================================
#include <JuceHeader.h>
//==============================================================================
namespace dmt {
namespace model {
//==============================================================================
static inline juce::AudioProcessorParameterGroup
oscilloscopeParameterGroup(juce::String parentUid)
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;

  juce::String uid = parentUid + "Oscilloscope";

  return juce::AudioProcessorParameterGroup(
    uid,            // group ID
    "Oscilloscope", // group name
    "|",            // separator
    std::make_unique<ParameterChoice>(
      uid + "Type",                               // parameter ID
      "Type",                                     // parameter name
      juce::StringArray{ "Normal", "Triggered" }, // choices
      0),                                         // defaultIndex
    std::make_unique<ParameterFloat>(uid + "Zoom",
                                     "Zoom",
                                     NormalisableRange(0.0f,   // rangeStart
                                                       100.0f, // rangeEnd
                                                       0.01f,  // intervalValue
                                                       1.0f),  // skewFactor
                                     25.0f),                   // defaultValue
    std::make_unique<ParameterFloat>(uid + "Gain",
                                     "Gain",
                                     NormalisableRange(-32.0f, // rangeStart
                                                       32.0f,  // rangeEnd
                                                       0.1f,   // intervalValue
                                                       1.0f),  // skewFactor
                                     0.0f),                    // defaultValue
    std::make_unique<ParameterFloat>(uid + "Thickness",
                                     "Thickness",
                                     NormalisableRange(0.5f,  // rangeStart
                                                       10.f,  // rangeEnd
                                                       .01f,  // intervalValue
                                                       1.0f), // skewFactor
                                     3.0f)                    // defaultValue

  );
}
} // namespace model
} // namespace dmt