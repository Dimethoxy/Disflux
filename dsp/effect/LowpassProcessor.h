//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ███████ ██    ██   ███     ████
 * ██   ██ ██ ██  ██  ██ ██         ██    ██   ██ ██    ██  ██ ██     ██
 * ██████  ██ ██      ██ ███████    ██    ██   ██  ██████  ██   ██    ██
 *
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * This file is part of the Dimethoxy Library, a collection of essential
 * classes used across various Dimethoxy projects.
 * These files are primarily designed for internal use within our repositories.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license. You must adhere GPLv3
 * license for any project using this code or parts of it. Your are not allowed
 * to use this code in any closed-source project.
 *
 * Description:
 * Lowpass Processor class for processing audio buffers with a low-pass filter.
 * This class has over the top comments because it also serves as a tutorial.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

// We use pragma once to ensure the header is only included once in the build.
#pragma once

//==============================================================================

// We include the JUCE header to gain access to the JUCE framework.
#include <JuceHeader.h>

//==============================================================================

// Some namespace shananigans to avoid conflicts with other libraries.
// Not really necessary but nice to structure the library.
namespace dmt {    // Dimethoxy Library Namespace
namespace dsp {    // Digital Signal Processing Module
namespace effect { // Effect Module

//==============================================================================
/**
 * @brief LowpassProcessor
 *
 * This class processes audio buffers with a series of low-pass filters.
 *
 * We use alignas(64) to align the class to 64 bytes as an attempt to optimize
 * the class in a cache-friendly manner. This isn't really necessary but can
 * improve cache performance in some cases.
 */
class alignas(64) LowpassProcessor
{
  //============================================================================
  // We define the minimum and maximum frequency values for the filters.
  // This uses constexpr to evaluate the values at compile-time.
  constexpr static float MIN_FREQUENCY = 20.0f;
  constexpr static float MAX_FREQUENCY = 20000.0f;

  //============================================================================
  // We define the maximum amount of filter stages we can have.
  // Each stage will add -6 dB/octave to the filter slope.
  // With 16 stages we can cover a range of -6 dB/octave to -96 dB/octave.
  // This uses constexpr to evaluate the values at compile-time.
  constexpr static int MIN_STAGES = 1;
  constexpr static int MAX_STAGES = 16;

  //============================================================================
  // We set some type aliases so we don't have to type out the full type names.
  // This makes for cleaner and more readable code.
  using AudioBuffer = juce::AudioBuffer<float>;
  using AudioProcessorValueTreeState = juce::AudioProcessorValueTreeState;
  using Filter = juce::IIRFilter;
  using FilterArray = std::array<Filter, MAX_STAGES>;

public:
  //============================================================================
  /**
   * @brief Constructs a LowpassProcessor with the given parameters.
   *
   * This function is called when an object of this class is created.
   * We use noexcept to indicate that this function won't throw exceptions.
   *
   * @param _apvts The AudioProcessorValueTreeState containing the parameters.
   *               This is a reference so no data is copied. (Indicated by &)
   *               This means that if the original object is changed, the
   *               changes will be reflected in this class automatically.
   *               This is useful for sharing data between classes. Otherwise,
   *               the data would be copied and changes to the original object
   *               wouldn't be reflected in this class. In that case we would
   *               have to pass down each change manually.
   */
  LowpassProcessor(AudioProcessorValueTreeState& _apvts) noexcept
    : apvts(_apvts)
  {
    // We don't really need to do anything here.
  }

  //============================================================================
  /**
   * @brief Prepares the processor with the given sample rate.
   *
   * This function prepares the processor with the given sample rate.
   * We need to know the sample rate to calculate the filter coefficients.
   * This function needs to be called before we can process audio.
   * Not calling this function first will result in failing to process audio.
   *
   * We inline this function to optimize performance.
   * We also use noexcept to indicate that this function won't throw exceptions.
   *
   * @param _newSampleRate The sample rate to prepare the processor with.
   */
  inline void prepare(const double _newSampleRate) noexcept
  {
    // We store the sample rate in a class variable so we can use it later.
    sampleRate = _newSampleRate;

    // Now that know the sample rate, we can calculate the filter coefficients.
    // We need those to start processing audio.
    setCoefficients();
  }

  //============================================================================
  /**
   * @brief Processes an audio buffer.
   *
   * This function processes an audio buffer with a low-pass filter.
   * Call this function from the AudioProcessor's processBlock() function.
   * Make sure to call prepare() before processing audio.
   * We inline this function to optimize performance.
   * We also use noexcept to indicate that this function won't throw exceptions.
   *
   * @param _buffer A reference to the audio buffer we want to process.
   *                This is a reference so no data is copied. (Indicated by &)
   *                If we didn't use a reference, the buffer would be copied
   *                when this function is called.
   */
  inline void processBlock(AudioBuffer& _buffer) noexcept
  {
    // We check if the sample rate is less than or equal to zero.
    // If it is we didn't call prepare() and we can't process the audio.
    if (sampleRate <= 0.0f) [[unlikely]] {
      return; // Exit the function early.
    }

    // We need to load the parameters from the AudioProcessorValueTreeState
    // We load the parameters into local variables so we can compare them
    // against the previous values and see if they have changed.
    // We also use clamp() to ensure the values stay within the valid range.
    const int newStages =
      std::clamp((int)apvts.getRawParameterValue("LowpassStages")->load(),
                 MIN_STAGES,
                 MAX_STAGES);
    const float newFrequency =
      std::clamp((float)apvts.getRawParameterValue("LowPassFrequency")->load(),
                 MIN_FREQUENCY,
                 MAX_FREQUENCY);

    // Check if the amount of stages has changed.
    const bool stagesChanged = stages != newStages;

    // Check if the frequency has changed.
    // We can't just compare floats with != because of floating point precision.
    // Therefore we use juce::approximatelyEqual() to compare the floats.
    const bool frequencyChanged =
      !juce::approximatelyEqual(frequency, newFrequency);

    // If either the stages or frequency have changed, we need to recalculate
    // the filters coefficients. Recalculating the coefficients is an expensive
    // operation so we only do it when the parameters have changed.
    if (stagesChanged || frequencyChanged) [[unlikely]] {
      stages = newStages;
      frequency = newFrequency;
      setCoefficients();
    }

    // We load the mix parameter from the AudioProcessorValueTreeState.
    // This one is irrelevant for the filter coefficients so we just save it.
    mix = apvts.getRawParameterValue("LowpassMix")->load();

    // Now we process the audio buffer with the low-pass filters.
    // We loop over each sample in the buffer and apply the filters.
    for (size_t sample = 0;
         sample < static_cast<size_t>(_buffer.getNumSamples());
         ++sample) {
      // Let's save the dry signal so we can mix it with the wet signal later.
      const float leftDry = _buffer.getSample(0, sample);
      const float rightDry = _buffer.getSample(1, sample);

      // For the wet signal we also start with the dry signal.
      float left = leftDry;
      float right = rightDry;

      // Now we loop over each filter stage and apply them to the audio.
      for (size_t filterIndex = 0; filterIndex < stages; ++filterIndex) {
        left = leftFilters[filterIndex].processSingleSampleRaw(left);
        right = rightFilters[filterIndex].processSingleSampleRaw(right);
      }

      // Now calculate the wet and dry gain
      const float wetGain = mix;
      const float dryGain = 1.0f - mix;

      // Next we mix the wet and dry signal together.
      left = (left * wetGain) + (leftDry * dryGain);
      right = (right * wetGain) + (rightDry * dryGain);

      // Finally we set the processed samples back into the audio buffer.
      _buffer.setSample(0, sample, left);
      _buffer.setSample(1, sample, right);
    }
  }

protected:
  //============================================================================
  /**
   * @brief Sets the coefficients for the filters.
   *
   * This function calculates the coefficients for the filters and sets them.
   * We use this function to update the filters when the parameters change.
   * This function is defined as protected so it can't be called from outside.
   * We inline this function to optimize performance.
   * We also use noexcept to indicate that this function won't throw exceptions.
   */
  inline void setCoefficients() noexcept
  {
    const auto coefficients =
      juce::IIRCoefficients::makeAllPass(sampleRate, frequency);

    for (size_t filterIndex = 0; filterIndex < MAX_STAGES; ++filterIndex) {
      leftFilters[filterIndex].setCoefficients(coefficients);
      rightFilters[filterIndex].setCoefficients(coefficients);
    }
  }

private:
  //============================================================================

  // A reference (indicated by &) to the APVTS of the plugin.
  AudioProcessorValueTreeState& apvts;

  // Tracks the sample rate of the audio.
  // We start with -1.0f to indicate that the sample rate is not set yet.
  double sampleRate = -1.0f;

  // Tracks the cutoff frequency of the filters.
  // We start with 800 Hz as a random default value.
  float frequency = 800.0f;

  // Tracks the mix between the dry and wet signal.
  // We start with 1.0f to indicate that the wet signal is fully active.
  float mix = 1.0f;

  // Tracks how many stages of the filters are active.
  // This will determine the slope of the filter.
  int stages = 1;

  // Our series of filters for the left and right channels.
  // They do the actual filtering of the audio.
  FilterArray leftFilters;
  FilterArray rightFilters;
};

//==============================================================================
// End of the namespaces
} // namespace effect
} // namespace dsp
} // namespace dmt