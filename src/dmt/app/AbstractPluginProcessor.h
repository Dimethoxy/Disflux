#pragma once

#include <JuceHeader.h>

namespace dmt {
namespace app {
class AbstractPluginProcessor : public juce::AudioProcessor
{
public:
  //==============================================================================
  AbstractPluginProcessor(
    std::function<juce::AudioProcessorValueTreeState::ParameterLayout()>
      createParameterLayout)
    : AudioProcessor(
        BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
          )
    , apvts(*this, nullptr, ProjectInfo::projectName, createParameterLayout())
  {
#if PERFETTO
    MelatoninPerfetto::get().beginSession();
#endif
  }

  //==============================================================================
  ~AbstractPluginProcessor() override
  {
#if PERFETTO
    MelatoninPerfetto::get().endSession();
#endif
  }

  //==============================================================================
  // Program Management

  double getTailLengthSeconds() const { return 0.0; }

  int getNumPrograms()
  {
    return 1; // NB: some hosts don't cope very well if you tell them there are
              // 0 programs, so this should be at least 1, even if you're not
              // really implementing programs.
  }

  int getCurrentProgram() { return 0; }

  void setCurrentProgram(int index) { juce::ignoreUnused(index); }

  const juce::String getProgramName(int index)
  {
    juce::ignoreUnused(index);
    return {};
  }

  void changeProgramName(int index, const juce::String& newName)
  {
    juce::ignoreUnused(index, newName);
  }

  //==============================================================================
  // Midi

  bool acceptsMidi() const override
  {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
  }
  bool producesMidi() const override
  {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
  }

  bool isMidiEffect() const override
  {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
  }

  //==============================================================================
  // Preset Save/Load

  void getStateInformation(juce::MemoryBlock& destData) override
  {
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
  }

  void setStateInformation(const void* data, int sizeInBytes) override
  {
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
      apvts.replaceState(tree);
    }
  }

  //==============================================================================
  //

  bool isBusesLayoutSupported(const BusesLayout& layouts) const
  {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
      return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
      return false;
#endif

    return true;
#endif
  }

  bool hasEditor() const
  {
    return true; // (change this to false if you choose to not supply an editor)
  }

public:
  //==============================================================================
  juce::AudioProcessorValueTreeState apvts;

private:
#if PERFETTO
  std::unique_ptr<perfetto::TracingSession> tracingSession;
#endif

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractPluginProcessor)
};
}
}