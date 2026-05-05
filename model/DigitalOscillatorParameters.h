#pragma once

#include "../dsp/synth/DigitalOscillator.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace model {
static inline juce::AudioProcessorParameterGroup
digitalOscillatorParameterGroup(juce::String parentUid)
{
  // using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;
  using ParameterGroup = juce::AudioProcessorParameterGroup;
  using String = juce::String;

  using DigitalWaveform = dmt::dsp::synth::DigitalWaveform;

  String uid = parentUid + "DigitalOscillator";

  return juce::AudioProcessorParameterGroup(
    uid,                                            // group ID
    "Waveform",                                     // group name
    "|",                                            // separator
    std::make_unique<ParameterChoice>(uid + "Type", // parameter ID
                                      "Type",       // parameter name
                                      DigitalWaveform::waveformNames, // choices
                                      2), // defaultValue
    std::make_unique<ParameterFloat>(uid + "Warp",
                                     "Warp",
                                     NormalisableRange(0.f,  // rangeStart
                                                       1.f,  // rangeEnd
                                                       .01f, // intervalValue
                                                       1.f), // skewFactor
                                     0.0f),
    std::make_unique<ParameterFloat>(uid + "Bend",
                                     "Bend",
                                     NormalisableRange(-100.f, // rangeStart
                                                       100.f,  // rangeEnd
                                                       .01f,   // intervalValue
                                                       1.f),   // skewFactor
                                     0.0f),                    // defaultValue
    std::make_unique<ParameterFloat>(uid + "Pwm",
                                     "Pwm",
                                     NormalisableRange(1.0f,  // rangeStart
                                                       100.f, // rangeEnd
                                                       .1f,   // intervalValue
                                                       1.f),  // skewFactor
                                     0.0f),                   // defaultValue
    std::make_unique<ParameterFloat>(uid + "Sync",
                                     "Sync",
                                     NormalisableRange(0.f,   // rangeStart
                                                       100.f, // rangeEnd
                                                       .01f,  // intervalValue
                                                       1.f),  // skewFactor
                                     .0f),                    // defaultValue
    std::make_unique<ParameterFloat>(uid + "Bias",
                                     "Bias",
                                     NormalisableRange(-1.f, // rangeStart
                                                       1.f,  // rangeEnd
                                                       .01f, // intervalValue
                                                       1.f), // skewFactor
                                     0.0f),                  // defaultValue
    std::make_unique<ParameterFloat>(uid + "Clip",
                                     "Clip",
                                     NormalisableRange(0.f,  // rangeStart
                                                       1.f,  // rangeEnd
                                                       .01f, // intervalValue
                                                       1.f), // skewFactor
                                     0.0f),                  // defaultValue
    std::make_unique<ParameterFloat>(uid + "Drive",
                                     "Drive",
                                     NormalisableRange(-20.f, // rangeStart
                                                       +20.f, // rangeEnd
                                                       .01f,  // intervalValue
                                                       1.f),  // skewFactor
                                     3.0f)                    // defaultValue
  );
}
} // namespace model
} // namespace dmt