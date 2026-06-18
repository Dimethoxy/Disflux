#pragma once

#include <JuceHeader.h>
#include "../dsp/synth/AnalogWaveform.h"

//==============================================================================
namespace dmt {
namespace model {
static inline juce::AudioProcessorParameterGroup
waveformParameterGroup(juce::String parentUid)
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;

  juce::String uid = parentUid + "Waveform";

  return juce::AudioProcessorParameterGroup(
    uid,                                            // group ID
    "Waveform",                          // group name
    "|",                                            // separator
    std::make_unique<ParameterChoice>(uid + "Type", // parameter ID
                                      "Type",       // parameter name
                                      dmt::dsp::synth::AnalogWaveform::waveformNames, // choices
                                      2), // defaultIndex
    std::make_unique<ParameterFloat>(uid + "Bend",
                                     "Bend",
                                     NormalisableRange(-100.f, // rangeStart
                                                       100.f,   // rangeEnd
                                                       .01f,   // intervalValue
                                                       1.f),  // skewFactor
                                     .0f),                    // defaultValue
    std::make_unique<ParameterFloat>(uid + "Pwm",
                                     "Pwm",
                                     NormalisableRange(.0f,  // rangeStart
                                                       100.f, // rangeEnd
                                                       .01f, // intervalValue
                                                       1.f), // skewFactor
                                     4.f),                   // defaultValue
    std::make_unique<ParameterFloat>(uid + "Sync",
                                     "Sync",
                                     NormalisableRange(0.f, // rangeStart
                                                       100.f,  // rangeEnd
                                                       .01f, // intervalValue
                                                       1.f), // skewFactor
                                     .0f)                   // defaultValue)                   
  );
}
} // namespace model
} // namespace dmt