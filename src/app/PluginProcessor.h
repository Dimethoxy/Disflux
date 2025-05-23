#pragma once

#include "AudioSettings.h"
#include <DmtHeader.h>
#include <JuceHeader.h>

//==============================================================================
class PluginProcessor final : public juce::AudioProcessor
{
public:
  //==============================================================================
  PluginProcessor();
  ~PluginProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  using AudioProcessor::processBlock;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  //==============================================================================
  juce::AudioProcessorValueTreeState apvts;

  //==============================================================================
  dmt::configuration::Properties properties;
  dmt::version::Manager versionManager;

  //==============================================================================
  dmt::dsp::data::FifoAudioBuffer<float> oscilloscopeBuffer;
  dmt::dsp::effect::DisfluxProcessor disfluxProcessor;

  //==============================================================================
  // Store scale factor for editor window
  float scaleFactor = 1.0f;
  float getScaleFactor() const { return scaleFactor; }
  void setScaleFactor(float newScale) { scaleFactor = newScale; }

private:
#if PERFETTO
  std::unique_ptr<perfetto::TracingSession> tracingSession;
#endif
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
