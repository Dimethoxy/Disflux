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
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * Interface for a ring buffer that provides easy and efficient access to
 * audio samples.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dsp/data/RingAudioBuffer.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace dsp {
namespace data {

//==============================================================================
/**
 * @brief Interface for a ring buffer that provides easy and efficient access to
 * audio samples.
 *
 * @tparam SampleType The type of audio sample (e.g., float, double).
 */
template<typename SampleType>
class alignas(64) RingBufferInterface
{
  using AudioBuffer = juce::AudioBuffer<SampleType>;

public:
  //============================================================================
  /**
   * @brief Constructs a RingBufferInterface with the given audio buffer and
   * positions.
   *
   * @param _audioBuffer The audio buffer to use.
   * @param _writePosition The write position in the buffer.
   * @param _readPosition The read positions for each channel.
   */
  constexpr RingBufferInterface(AudioBuffer& _audioBuffer,
                                const int& _writePosition,
                                std::vector<int>& _readPosition) noexcept
    : audioBuffer(_audioBuffer)
    , writePosition(_writePosition)
    , readPosition(_readPosition)
  {
  }

  //============================================================================
  /**
   * @brief Retrieves a sample from the buffer.
   *
   * @param _channel The channel to read from.
   * @param _sample The sample index to read.
   * @return The sample value.
   */
  forcedinline SampleType getSample(const int _channel,
                                    const int _sample) const noexcept
  {
    const int numSamples = audioBuffer.getNumSamples();
    const int block1size = numSamples - writePosition;
    if (_sample < block1size) [[likely]] {
      return audioBuffer.getSample(_channel, writePosition + _sample);
    }
    return audioBuffer.getSample(_channel, _sample - block1size);
  }

  //============================================================================
  /**
   * @brief Gets the read position for a specific channel.
   *
   * @param _channel The channel index.
   * @return The read position.
   */
  forcedinline int getReadPosition(int _channel) const noexcept
  {
    const int numSamples = audioBuffer.getNumSamples();
    const int rawReadPosition = readPosition[_channel];
    if (rawReadPosition > writePosition) [[likely]] {
      return rawReadPosition - writePosition;
    }
    return numSamples - writePosition + rawReadPosition;
  }

  //============================================================================
  /**
   * @brief Increments the read position for a specific channel.
   *
   * @param _channel The channel index.
   * @param _increment The amount to increment.
   */
  forcedinline void incrementReadPosition(int _channel, int _increment) noexcept
  {
    const int numSamples = audioBuffer.getNumSamples();
    const int oldReadPosition = readPosition[_channel];
    const int newReadPosition = (oldReadPosition + _increment) % numSamples;
    readPosition[_channel] = newReadPosition;
  }

  //============================================================================
  /**
   * @brief Sets the raw read position for a specific channel.
   *
   * @param _channel The channel index.
   * @param _position The new read position.
   */
  forcedinline void setRawReadPosition(const int _channel,
                                       const int _position) noexcept
  {
    readPosition[_channel] = _position;
  }

  //============================================================================
  /**
   * @brief Gets the raw read position for a specific channel.
   *
   * @param _channel The channel index.
   * @return The raw read position.
   */
  forcedinline int getRawReadPosition(const int _channel) const noexcept
  {
    return readPosition[_channel];
  }

  //============================================================================
  /**
   * @brief Equalizes the read positions across all channels.
   */
  forcedinline void equalizeReadPositions() noexcept
  {
    int highestReadPosition = 0;
    int highestReadChannel = 0;

    // Find the highest read position among all channels
    for (size_t channel = 0; channel < audioBuffer.getNumChannels();
         ++channel) {
      const int readPos = getReadPosition(static_cast<int>(channel));
      if (readPos > highestReadPosition) [[likely]] {
        highestReadPosition = readPos;
        highestReadChannel = static_cast<int>(channel);
      }
    }

    // Get the raw read position for the channel with the highest read position
    const int highestRawReadPosition = getRawReadPosition(highestReadChannel);

    // Set all channels to the highest raw read position found
    for (size_t channel = 0; channel < audioBuffer.getNumChannels();
         ++channel) {
      setRawReadPosition(static_cast<int>(channel), highestRawReadPosition);
    }
  }

private:
  AudioBuffer& audioBuffer;
  const int& writePosition;
  std::vector<int>& readPosition;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RingBufferInterface)
};

} // namespace data
} // namespace dsp
} // namespace dmt