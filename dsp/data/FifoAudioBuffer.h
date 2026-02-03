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
 * A lock-free FIFO audio buffer optimized for real-time performance.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace dsp {
namespace data {

//==============================================================================
/**
 * @brief A lock-free FIFO audio buffer optimized for real-time performance.
 *
 * This class uses a circular buffer to manage audio data in a lock-free manner.
 *
 * @tparam SampleType The type of audio sample (e.g., float, double).
 */
template<typename SampleType>
class alignas(64) FifoAudioBuffer : public juce::AbstractFifo
{
  using BufferData = std::vector<std::vector<float>>;
  using ChannelData = std::vector<float>;

public:
  //============================================================================
  /**
   * @brief Constructs a FifoAudioBuffer with the specified number of channels
   * and buffer size.
   *
   * @param _channels The number of audio channels.
   * @param _bufferSize The size of the buffer.
   */
  constexpr inline FifoAudioBuffer(int _channels, int _bufferSize) noexcept
    : AbstractFifo(_bufferSize)
  {
    buffer.setSize(_channels, _bufferSize);
  }

  //============================================================================
  /**
   * @brief Adds audio data to the FIFO buffer.
   *
   * @param _target The audio buffer containing the data to add.
   */
  forcedinline void addToFifo(
    const juce::AudioBuffer<SampleType>& _target) noexcept
  {
    const int numSamples = _target.getNumSamples();
    int firstBlockStart, firstBlockSize, secondBlockStart, secondBlockSize;

    prepareToWrite(numSamples,
                   firstBlockStart,
                   firstBlockSize,
                   secondBlockStart,
                   secondBlockSize);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
      if (firstBlockSize > 0)
        buffer.copyFrom(
          channel, firstBlockStart, _target, channel, 0, firstBlockSize);
      if (secondBlockSize > 0)
        buffer.copyFrom(channel,
                        secondBlockStart,
                        _target,
                        channel,
                        firstBlockSize,
                        secondBlockSize);
    }

    finishedWrite(firstBlockSize + secondBlockSize);
  }

  //============================================================================
  /**
   * @brief Reads audio data from the FIFO buffer.
   *
   * @param _target The audio buffer to store the read data.
   */
  forcedinline void readFromFifo(
    juce::AudioBuffer<SampleType>& _target) noexcept
  {
    const int numSamples = _target.getNumSamples();
    int firstBlockStart, firstBlockSize, secondBlockStart, secondBlockSize;

    prepareToRead(numSamples,
                  firstBlockStart,
                  firstBlockSize,
                  secondBlockStart,
                  secondBlockSize);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
      if (firstBlockSize > 0)
        _target.copyFrom(
          channel, 0, buffer, channel, firstBlockStart, firstBlockSize);
      if (secondBlockSize > 0)
        _target.copyFrom(channel,
                         firstBlockSize,
                         buffer,
                         channel,
                         secondBlockStart,
                         secondBlockSize);
    }

    finishedRead(firstBlockSize + secondBlockSize);
  }

  //============================================================================
  /**
   * @brief Resizes the FIFO buffer.
   *
   * @param _channels The new number of channels.
   * @param _newBufferSize The new buffer size.
   */
  forcedinline void setSize(const int _channels,
                            const int _newBufferSize) noexcept
  {
    buffer.setSize(_channels, _newBufferSize);
    setTotalSize(_newBufferSize);
    reset();
  }

  //============================================================================
  /**
   * @brief Clears the FIFO buffer.
   */
  forcedinline void clear() noexcept
  {
    buffer.clear();
    reset();
  }

  //============================================================================
  /**
   * @brief Gets the number of channels in the FIFO buffer.
   *
   * @return The number of channels.
   */
  forcedinline int getNumChannels() const noexcept
  {
    return buffer.getNumChannels();
  }

  //============================================================================
  /**
   * @brief Gets the number of samples in the FIFO buffer.
   *
   * @return The number of samples.
   */
  forcedinline int getNumSamples() const noexcept
  {
    return buffer.getNumSamples();
  }

  //============================================================================
  /**
   * @brief Gets the underlying audio buffer.
   *
   * @return A const reference to the audio buffer.
   */
  forcedinline const juce::AudioBuffer<SampleType>& getBuffer() const noexcept
  {
    return buffer;
  }

private:
  juce::AudioBuffer<SampleType> buffer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FifoAudioBuffer)
};

} // namespace data
} // namespace dsp
} // namespace dmt
