#include <JuceHeader.h>
//==============================================================================
namespace dmt {
namespace model {
static inline juce::AudioProcessorParameterGroup
oscSendParameterGroup(juce::String parentUid, juce::String channel)
{
  using ParameterFloat = juce::AudioParameterFloat;
  using NormalisableRange = juce::NormalisableRange<float>;

  juce::String uid = parentUid + "Send" + channel;

  return juce::AudioProcessorParameterGroup(
    uid,                                                      // group ID
    "OscSend",                                                // group name
    "|",                                                      // separator
    std::make_unique<ParameterFloat>(uid + "Gain",            // parameter ID
                                     "Gain",                  // parameter name
                                     NormalisableRange(-96.f, // rangeStart
                                                       0.f,   // rangeEnd
                                                       .1f,   // intervalValue
                                                       1.f),  // skewFactor
                                     0.f),
    std::make_unique<ParameterFloat>(uid + "Pan",            // parameter ID
                                     "Pan",                  // parameter name
                                     NormalisableRange(-1.f, // rangeStart
                                                       1.f,  // rangeEnd
                                                       .01f, // intervalValue
                                                       1.f), // skewFactor
                                     0.f));                  // defaultValue
}

} // namespace model
} // namespace dmt