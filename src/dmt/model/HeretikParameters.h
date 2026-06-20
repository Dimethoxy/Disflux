#pragma once

//==============================================================================

#include <JuceHeader.h>
#include <dmt/utility/Unit.h>

//==============================================================================

namespace dmt {
namespace model {

//==============================================================================

static inline juce::AudioProcessorParameterGroup
heretikParameterGroup(juce::String parentUid, int versionHint)
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  // using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;
  using Unit = dmt::utility::Unit;
  juce::String uid = parentUid + "Heretik";

  return juce::AudioProcessorParameterGroup(
    uid,       // group ID
    "Heretik", // group name
    "|",       // separator
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "PreGain", versionHint),
                                     "PreGain",
                                     NormalisableRange(-32.0f, // rangeStart
                                                       32.0f,  // rangeEnd
                                                       0.01f,  // intervalValue
                                                       1.0f),  // skewFactor
                                     0.0f),                    // defaultValue
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "Spread", versionHint),
                                     "Spread",
                                     NormalisableRange(0.0f,  // rangeStart
                                                       1.0f,  // rangeEnd
                                                       0.01f, // intervalValue
                                                       1.0f), // skewFactor
                                     0.5f),                   // defaultValue
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "Range", versionHint),
                                     "Range",
                                     NormalisableRange(0.0f,  // rangeStart
                                                       16.0f, // rangeEnd
                                                       0.01f, // intervalValue
                                                       1.0f), // skewFactor
                                     8.0f),                   // defaultValue
    // defaultValue
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "Mix", versionHint),
                                     "Mix",
                                     NormalisableRange(0.0f,  // rangeStart
                                                       1.0f,  // rangeEnd
                                                       0.01f, // intervalValue
                                                       1.0f), // skewFactor
                                     0.5f),                   // defaultValue
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "Distortion", versionHint),
                                     "Distortion",
                                     NormalisableRange(0.0f,  // rangeStart
                                                       1.0f,  // rangeEnd
                                                       0.01f, // intervalValue
                                                       1.0f), // skewFactor
                                     0.1f),                   // defaultValue
    //==============================================================================
    // Heretik Drive parameters
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "Drive", versionHint),
                                     "Drive",
                                     NormalisableRange(0.0f,  // rangeStart
                                                       1.0f,  // rangeEnd
                                                       0.01f, // intervalValue
                                                       1.0f), // skewFactor
                                     0.5f),                   // defaultValue
    std::make_unique<AudioParameterChoice>(juce::ParameterID(uid + "DriveType", versionHint),
                                           "Drive Type",
                                           juce::StringArray{ "Hard Clipping",
                                                              "Soft Clipping",
                                                              "Saturate",
                                                              "Atan",
                                                              "Crunch",
                                                              "Bitcrush",
                                                              "Extreme",
                                                              "Screame",
                                                              "Sine",
                                                              "Cosine",
                                                              "Harmonic",
                                                              "Weird" },
                                           0), // defaultItemIndex
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "DriveBias", versionHint),
                                     "Drive Bias",
                                     NormalisableRange(-1.0f, // rangeStart
                                                       1.0f,  // rangeEnd
                                                       0.01f, // intervalValue
                                                       1.0f), // skewFactor
                                     0.0f),                   // defaultValue
    //==============================================================================
    // Heretik Feedback parameters
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "Feedback", versionHint),
                                     "Feedback",
                                     NormalisableRange(0.0f,  // rangeStart
                                                       1.0f,  // rangeEnd
                                                       0.01f, // intervalValue
                                                       1.0f), // skewFactor
                                     0.0f),                   // defaultValue
    std::make_unique<AudioParameterChoice>(
      juce::ParameterID(uid + "FeedbackFilterSlope", versionHint), // parameter ID
      "Feedback Filter Slope",                                     // parameter name
      juce::StringArray{ Unit::multiFilterSlopes },                // choices
      0),                                                          // defaultItemIndex
    std::make_unique<ParameterFloat>(juce::ParameterID(uid + "FeedbackFilterCutoff", versionHint),
                                     "Feedback Filter Cutoff",
                                     NormalisableRange(20.0f,    // rangeStart
                                                       20000.0f, // rangeEnd
                                                       1.0f,   // intervalValue
                                                       0.25f), // skewFactor
                                     20000.0f));
}
} // namespace model
} // namespace dmt