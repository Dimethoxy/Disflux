#pragma once

#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace model {
static inline juce::AudioProcessorParameterGroup
envelopeParameterGroup(juce::String parentUid,
                       juce::String suffix,
                       std::array<float, 6> defaultValues)
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;

  juce::String uid = parentUid + suffix + "Env";

  return juce::AudioProcessorParameterGroup(
    uid,                                               // group ID
    suffix + "Envelope",                               // group name
    "|",                                               // separator
    std::make_unique<ParameterChoice>(uid + "Enabled", // parameter ID
                                      "Enabled",       // parameter name
                                      StringArray{ "Off", "On" }, // choices
                                      1),                      // defaultValue
    std::make_unique<ParameterFloat>(uid + "Attack",           // parameter ID
                                     "Attack",                 // parameter name
                                     NormalisableRange(0.0f,   // rangeStart
                                                       0.3f,   // rangeEnd
                                                       0.001f, // intervalValue
                                                       0.5f),  // skewFactor
                                     defaultValues[0]),        // defaultValue
    std::make_unique<ParameterFloat>(uid + "Hold",             // parameter ID
                                     "Hold",                   // parameter name
                                     NormalisableRange(0.0f,   // rangeStart
                                                       0.3f,   // rangeEnd
                                                       0.001f, // intervalValue
                                                       0.5f),  // skewFactor
                                     defaultValues[1]),        // defaultValue
    std::make_unique<ParameterFloat>(uid + "Decay",            // parameter ID
                                     "Decay",                  // parameter name
                                     NormalisableRange(0.0f,   // rangeStart
                                                       1.0f,   // rangeEnd
                                                       0.001f, // intervalValue
                                                       0.5f),  // skewFactor
                                     defaultValues[2]),        // defaultValue
    std::make_unique<ParameterFloat>(uid + "Depth",            // parameter ID
                                     "Depth",                  // parameter name
                                     NormalisableRange(0.0f,   // rangeStart
                                                       1.0f,   // rangeEnd
                                                       0.001f, // intervalValue
                                                       0.5f),  // skewFactor
                                     defaultValues[3]),        // defaultValue
    std::make_unique<ParameterFloat>(uid + "AttackBend",       // parameter ID
                                     "Attack Bend",            // parameter name
                                     NormalisableRange(-20.0f, // rangeStart
                                                       20.0f,  // rangeEnd
                                                       0.1f,   // intervalValue
                                                       1.0f),  // skewFactor
                                     defaultValues[4]),        // defaultValue
    std::make_unique<ParameterFloat>(uid + "DecayBend",        // parameter ID
                                     "Decay Bend",             // parameter name
                                     NormalisableRange(-20.0f, // rangeStart
                                                       20.0f,  // rangeEnd
                                                       0.1f,   // intervalValue
                                                       1.0f),  // skewFactor
                                     defaultValues[5])         // defaultValue
  );
}
} // namespace model
} // namespace dmt