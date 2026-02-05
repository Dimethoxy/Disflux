#include "PluginProcessor.h"
#include "ParameterLayout.h"
#include "PluginEditor.h"
//==============================================================================
PluginProcessor::PluginProcessor()
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
  , oscilloscopeBuffer(2, 4096)
#if DMT_EXCLUDE_DISFLUX_PROCESSING == 0
  , disfluxProcessor(apvts,
                     dmt::Settings::Audio::frequencySmoothness,
                     dmt::Settings::Audio::spreadSmoothness,
                     dmt::Settings::Audio::pinchSmoothness,
                     dmt::Settings::Audio::useOutputHighpass,
                     dmt::Settings::Audio::outputHighpassFrequency,
                     dmt::Settings::Audio::smoothingInterval)
#endif
{
#if PERFETTO
  MelatoninPerfetto::get().beginSession();
#endif
  properties.initialize();
}

PluginProcessor::~PluginProcessor()
{
#if PERFETTO
  MelatoninPerfetto::get().endSession();
#endif
}

//==============================================================================
const juce::String
PluginProcessor::getName() const
{
  return "Disflux";
}

bool
PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool
PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool
PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double
PluginProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

int
PluginProcessor::getNumPrograms()
{
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int
PluginProcessor::getCurrentProgram()
{
  return 0;
}

void
PluginProcessor::setCurrentProgram(int index)
{
  juce::ignoreUnused(index);
}

const juce::String
PluginProcessor::getProgramName(int index)
{
  juce::ignoreUnused(index);
  return {};
}

void
PluginProcessor::changeProgramName(int index, const juce::String& newName)
{
  juce::ignoreUnused(index, newName);
}

//==============================================================================
void
PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
  juce::ignoreUnused(sampleRate, samplesPerBlock);

#if DMT_EXCLUDE_DISFLUX_PROCESSING == 0
  disfluxProcessor.prepare(sampleRate);
#endif
}

void
PluginProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

bool
PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void
PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                              juce::MidiBuffer& midiMessages)
{
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  //============================================================================
  TRACE_DSP();
  const auto* bypassParam = apvts.getRawParameterValue("GlobalBypass");
  bool isBypassed = bypassParam->load() > 0.5f;

  if (!isBypassed) {
#if DMT_EXCLUDE_DISFLUX_PROCESSING == 0
    disfluxProcessor.processBlock(buffer);
#endif
  }
  oscilloscopeBuffer.addToFifo(buffer);
}

//==============================================================================
bool
PluginProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor*
PluginProcessor::createEditor()
{
  return new PluginEditor(*this);
}

//==============================================================================
void
PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
  juce::MemoryOutputStream mos(destData, true);
  apvts.state.writeToStream(mos);
}

void
PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
  auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
  if (tree.isValid()) {
    apvts.replaceState(tree);
  }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE
createPluginFilter()
{
  return new PluginProcessor();
}
