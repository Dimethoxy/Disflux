#include "PluginProcessor.h"
#include "ParameterLayout.h"
#include "PluginEditor.h"
//==============================================================================
PluginProcessor::PluginProcessor()
  : dmt::app::AbstractPluginProcessor(createParameterLayout)
  , oscilloscopeBuffer(2, 4096)
  , disfluxProcessor(apvts,
                     dmt::Settings::Audio::frequencySmoothness,
                     dmt::Settings::Audio::pinchSmoothness,
                     dmt::Settings::Audio::spreadSmoothness,
                     dmt::Settings::Audio::useOutputHighpass,
                     dmt::Settings::Audio::outputHighpassFrequency,
                     dmt::Settings::Audio::smoothingInterval)
{
}

PluginProcessor::~PluginProcessor() = default;

//==============================================================================
const juce::String
PluginProcessor::getName() const
{
  return "Disflux";
}

//==============================================================================
void
PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
  juce::ignoreUnused(sampleRate, samplesPerBlock);

  disfluxProcessor.prepare(sampleRate);
}

//==============================================================================
void
PluginProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

//==============================================================================
void
PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                              juce::MidiBuffer& midiMessages)
{
  // Boilerplate
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Start actual processing
  TRACE_DSP();
  const auto* bypassParam = apvts.getRawParameterValue("GlobalBypass");
  bool isBypassed = bypassParam->load() > 0.5f;

  if (!isBypassed) {
    disfluxProcessor.processBlock(buffer);
  }
  oscilloscopeBuffer.addToFifo(buffer);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessorEditor*
PluginProcessor::createEditor()
{
  return new PluginEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE
createPluginFilter()
{
  return new PluginProcessor();
}
