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
  dmt::configuration::Properties properties;
  dmt::version::Manager versionManager;

  //==============================================================================
  dmt::dsp::data::FifoAudioBuffer<float> oscilloscopeBuffer;
  dmt::dsp::effect::DisfluxProcessor disfluxProcessor;

  //==============================================================================
  // Store scale factor for editor window
  float sizeFactor = 1.0f;
  float getSizeFactor() const { return sizeFactor; }
  void setSizeFactor(float newSize) { sizeFactor = newSize; }

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
