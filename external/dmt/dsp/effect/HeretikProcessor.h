//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>
#include <utility/Settings.h>

//==============================================================================

namespace dmt {
namespace dsp {
namespace effect {

//==============================================================================

/**
 * @brief Disflux Processor
 *
 * This class processes audio buffers with a series of all-pass filters.
 */
class alignas(64) HeretikProcessor
{
  using AudioBuffer = juce::AudioBuffer<float>;
  using DelayLine = juce::dsp::DelayLine<float>;
  using Filter = juce::IIRFilter;

  // Add constexprs for min and max delay times (in ms)
  static constexpr float maxDelayMs = 240.0f;
  static constexpr float minDelayMs = 1.0f;

public:
  //==============================================================================
  /**
   * @brief Constructs a DisfluxProcessor with the given parameters.
   *
   * @param _apvts The AudioProcessorValueTreeState containing the parameters.
   */
  HeretikProcessor(juce::AudioProcessorValueTreeState& _apvts) noexcept
    : apvts(_apvts)
  {
  }

  //==============================================================================
  /**
   * @brief Prepares the processor with the given sample rate.
   *
   * @param _newSampleRate The sample rate.
   */
  inline void prepare(const double _newSampleRate,
                      const int _samplesPerBlock) noexcept
  {
    sampleRate = static_cast<float>(_newSampleRate);
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = _samplesPerBlock;
    spec.numChannels = 2;
    delayLine.prepare(spec);
    delayLine.setMaximumDelayInSamples((int)sampleRate);
  }

  //==============================================================================
  /**
   * @brief Processes an audio buffer.
   *
   * @param _buffer The audio buffer.
   */
  inline void processBlock(AudioBuffer& _buffer) noexcept
  {
    if (sampleRate <= 0.0f) {
      return;
    }

    const float drive = apvts.getRawParameterValue("HeretikDrive")->load();
    const float range = apvts.getRawParameterValue("HeretikRange")->load();
    const float tone = apvts.getRawParameterValue("HeretikTone")->load();
    const float feedback =
      apvts.getRawParameterValue("HeretikFeedback")->load();
    const float mix = apvts.getRawParameterValue("HeretikMix")->load();

    // Applay the delay line to the input buffer
    for (int channel = 0; channel < _buffer.getNumChannels(); ++channel) {
      Filter& filter = channel == 0 ? leftFilter : rightFilter;
      filter.setCoefficients(
        juce::IIRCoefficients::makeLowPass(sampleRate, tone, 0.5f));
      auto* channelData = _buffer.getWritePointer(channel);
      for (int sample = 0; sample < _buffer.getNumSamples(); ++sample) {
        // Dry signal
        const float drySample = channelData[sample] + feedbackBuffer[channel];
        delayLine.pushSample(channel, drySample);

        // Set delay time
        const int delayInSamples =
          getDelayInSamples(drySample, drive, range, filter);
        delayLine.setDelay(delayInSamples);

        // Output
        const float wetSample = delayLine.popSample(channel);
        const float mixSample = (wetSample * mix) + (drySample * (1.0f - mix));
        channelData[sample] = mixSample;
        feedbackBuffer[channel] = wetSample * feedback;
      }
    }
  }

protected:
  int getDelayInSamples(const float _drySample,
                        const float _drive,
                        const float _range,
                        Filter& _filter) const noexcept
  {
    const float filteredSample = _filter.processSingleSampleRaw(_drySample);
    const float driveSample = filteredSample * _drive;
    const float clampedSample = std::clamp(driveSample, -1.0f, 1.0f);
    const float denormalizedSample = (clampedSample + 1.0f) * 0.5f;
    const float multipliedSample = denormalizedSample * _range;
    const int delayInMs = msInSamples(multipliedSample);
    return std::clamp(delayInMs, 0, (int)sampleRate);
  }

  int msInSamples(float ms) const noexcept
  {
    return static_cast<int>(juce::jlimit(minDelayMs, maxDelayMs, ms) *
                            sampleRate / 1000.0f);
  }

private:
  //==============================================================================
  juce::AudioProcessorValueTreeState& apvts;
  DelayLine delayLine;
  float sampleRate = -1.0f;
  std::array<float, 2> feedbackBuffer;
  Filter leftFilter;
  Filter rightFilter;
};

//==============================================================================
} // namespace effect
} // namespace dsp
} // namespace dmt