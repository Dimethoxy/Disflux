#pragma once

#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace model {
static inline juce::AudioProcessorParameterGroup
voiceParameterGroup(juce::String parentUid)
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;

  juce::String uid = parentUid + "Voice";

  return juce::AudioProcessorParameterGroup(
    uid,                                                       // group ID
    "Voice",                                                   // group name
    "|",                                                       // separator
    std::make_unique<ParameterInt>(uid + "Octave",             // parameter ID
                                   "Octave",                   // parameter name
                                   -4,                         // rangeStart
                                   4,                          // rangeEnd
                                   0),                         // defaultValue
    std::make_unique<ParameterInt>(uid + "Semitone",           // parameter ID
                                   "Semitones",                // parameter name
                                   0,                          // rangeStart
                                   11,                         // rangeEnd
                                   0),                         // defaultValue
    std::make_unique<ParameterFloat>(uid + "Fine",             // parameter ID
                                     "Fine",                   // parameter name
                                     NormalisableRange(-100.f, // rangeStart
                                                       100.f,  // rangeEnd
                                                       .1f,    // intervalValue
                                                       1.f),   // skewFactor
                                     0),
    std::make_unique<ParameterInt>(uid + "Density",         // parameter ID
                                   "Density",               // parameter name
                                   1,                       // rangeStart
                                   8,                       // rangeEnd
                                   1),                      // defaultValue
    std::make_unique<ParameterFloat>(uid + "Detune",        // parameter ID
                                     "Detune",              // parameter name
                                     NormalisableRange(0.f, // rangeStart
                                                       100.f,
                                                       .1f,
                                                       1.f),
                                     0.f),
    std::make_unique<ParameterChoice>(uid + "Distribution", // parameter ID
                                      "Distribution",       // parameter name
                                      juce::StringArray{ "Linear",
                                                         "Quadratic",
                                                         "Cubic",
                                                         "Octic",
                                                         "Square Root",
                                                         "Cube Root",
                                                         "Octic Root",
                                                         "Sine",
                                                         "Random" }, // choices
                                      0),                     // default index
    std::make_unique<ParameterFloat>(uid + "Blend",           // parameter ID
                                     "Blend",                 // parameter name
                                     NormalisableRange(0.f,   // rangeStart
                                                       100.f, // rangeEnd
                                                       .1f,   // intervalValue
                                                       1.f),  // skewFactor
                                     0.f),                    // defaultValue
    std::make_unique<ParameterFloat>(uid + "Width",           // parameter ID
                                     "Width",                 // parameter name
                                     NormalisableRange(0.f,   // rangeStart
                                                       100.f, // rangeEnd
                                                       .1f,   // intervalValue
                                                       1.f),  // skewFactor
                                     0.f),                    // defaultValue
    std::make_unique<ParameterChoice>(
      uid + "Seed", // parameter ID
      "Seed",       // parameter name
      juce::StringArray{ "Random",
                         "Equal",
                         "Static #1",
                         "Static #2",
                         "Static #3",
                         "Static #4",
                         "Static #5" },                       // choices
      0),                                                     // default index
    std::make_unique<ParameterFloat>(uid + "Random",          // parameter ID
                                     "Random",                // parameter name
                                     NormalisableRange(0.f,   // rangeStart
                                                       100.f, // rangeEnd
                                                       .1f,   // intervalValue
                                                       1.f),  // skewFactor
                                     0.f),
    std::make_unique<ParameterFloat>(uid + "Phase",           // parameter ID
                                     "Phase",                 // parameter name
                                     NormalisableRange(0.f,   // rangeStart
                                                       100.f, // rangeEnd
                                                       .1f,   // intervalValue
                                                       1.f),  // skewFactor
                                     0.f));                   // defaultValue
}
} // namespace model
} // namespace dmt