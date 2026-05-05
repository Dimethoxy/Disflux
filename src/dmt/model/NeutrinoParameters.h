#pragma once
//==============================================================================
#include "AhdEnvelopeParameters.h"
#include "DigitalOscillatorParameters.h"
#include <JuceHeader.h>
//==============================================================================
namespace dmt {
namespace model {
//==============================================================================
static inline juce::AudioProcessorParameterGroup
neutrinoParameterGroup(juce::String parentUid, [[maybe_unused]] int versionHint)
{
  using ParameterInt = juce::AudioParameterInt;
  using ParameterFloat = juce::AudioParameterFloat;
  using ParameterChoice = juce::AudioParameterChoice;
  using NormalisableRange = juce::NormalisableRange<float>;
  using ParameterGroup = juce::AudioProcessorParameterGroup;

  juce::String uid = parentUid + "Neutrino";

  return juce::AudioProcessorParameterGroup(
    uid,        // group ID
    "Neutrino", // group name
    "|",        // separator

    // Oscillators
    std::make_unique<ParameterGroup>(digitalOscillatorParameterGroup(uid)),

    // Envelopes
    std::make_unique<ParameterGroup>(envelopeParameterGroup(
      uid, "Gain", { 0.0f, 0.055f, 0.350f, 0.0f, 0.0f, 0.0f })),
    std::make_unique<ParameterGroup>(envelopeParameterGroup(
      uid, "Pitch1", { 0.0f, 0.0f, 0.185f, 0.033f, 0.0f, 0.0f })),
    std::make_unique<ParameterGroup>(envelopeParameterGroup(
      uid, "Pitch2", { 0.0f, 0.0f, 0.02f, 0.033f, 0.0f, 0.0f })));
}
} // namespace model
} // namespace dmt