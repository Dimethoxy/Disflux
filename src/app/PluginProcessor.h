#pragma once

#include "AudioSettings.h"
#include <DmtHeader.h>
#include <JuceHeader.h>

//==============================================================================
class PluginProcessor final : public dmt::app::AbstractPluginProcessor
{
public:
  //==============================================================================
  PluginProcessor();
  ~PluginProcessor() override;

  //==============================================================================
  const juce::String getName() const override;
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;

  //==============================================================================
  dmt::dsp::data::FifoAudioBuffer<float> oscilloscopeBuffer;
  dmt::dsp::effect::DisfluxProcessor disfluxProcessor;

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
