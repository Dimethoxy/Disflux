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
 * A ring buffer for audio data that supports efficient writing and
 * reading.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dsp/data/FifoAudioBuffer.h"
#include "dsp/data/RingBufferInterface.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace dsp {
namespace data {

//==============================================================================
/**
 * @brief A ring buffer for audio data that supports efficient writing and
 * reading.
 *
 * @tparam SampleType The type of audio sample (e.g., float, double).
 */
template<typename SampleType>
class alignas(64) RingAudioBuffer : public RingBufferInterface<SampleType>
{
  using AudioBuffer = juce::AudioBuffer<SampleType>;
  using FifoAudioBuffer = dmt::dsp::data::FifoAudioBuffer<SampleType>;
  using RingBufferInterface = dmt::dsp::data::RingBufferInterface<SampleType>;

public:
  //============================================================================
  /**
   * @brief Constructs a RingAudioBuffer with the specified number of channels
   * and samples.
   *
   * @param _numChannelsToAllocate The number of channels to allocate.
   * @param _numSamplesToAllocate The number of samples to allocate.
   */
  constexpr RingAudioBuffer(const int _numChannelsToAllocate,
                            const int _numSamplesToAllocate) noexcept
    : RingBufferInterface(ringBuffer, writePosition, readPositions)
    , writePosition(0)
    , ringBuffer(_numChannelsToAllocate, _numSamplesToAllocate)
    , readPositions(static_cast<size_t>(_numChannelsToAllocate), 0)
  {
  }

  //============================================================================
  /**
   * @brief Writes audio data to the ring buffer.
   *
   * @param _bufferToWrite The audio buffer containing the data to write.
   */
  forcedinline void write(const AudioBuffer& _bufferToWrite) noexcept
  {
    const int numChannels = getNumChannels();
    const int bufferSize = getNumSamples();
    const int channelsToWrite = _bufferToWrite.getNumChannels();
    const int samplesToWrite = _bufferToWrite.getNumSamples();

    if (channelsToWrite > numChannels || samplesToWrite > bufferSize)
      [[unlikely]] {
      jassertfalse;
      return;
    }

    const int firstBlockSize = getNumSamples() - writePosition;
    const int secondBlockSize = samplesToWrite - firstBlockSize;

    for (size_t channel = 0; channel < static_cast<size_t>(channelsToWrite);
         ++channel) {
      if (firstBlockSize > 0)
        ringBuffer.copyFrom(static_cast<int>(channel),
                            writePosition,
                            _bufferToWrite,
                            static_cast<int>(channel),
                            0,
                            firstBlockSize);
      if (secondBlockSize > 0)
        ringBuffer.copyFrom(static_cast<int>(channel),
                            0,
                            _bufferToWrite,
                            static_cast<int>(channel),
                            firstBlockSize,
                            secondBlockSize);
    }

    updateWritePosition(samplesToWrite);
  }

  //============================================================================
  /**
   * @brief Writes audio data from a FIFO buffer to the ring buffer.
   *
   * @param _bufferToWrite The FIFO buffer containing the data to write.
   */
  forcedinline void write(FifoAudioBuffer& _bufferToWrite) noexcept
  {
    const int numChannels = getNumChannels();
    const int bufferSize = getNumSamples();
    const int channelsToWrite = _bufferToWrite.getNumChannels();
    const int samplesToWrite = _bufferToWrite.getNumReady();

    if (channelsToWrite > numChannels || samplesToWrite > bufferSize)
      [[unlikely]] {
      jassertfalse;
      return;
    }

    const AudioBuffer& source = _bufferToWrite.getBuffer();
    int start1, size1, start2, size2;
    _bufferToWrite.prepareToRead(samplesToWrite, start1, size1, start2, size2);

    // Block 1 Section 1
    if (size1 > 0) {
      int section1size = jmin(size1, bufferSize - writePosition);
      for (size_t channel = 0; channel < static_cast<size_t>(channelsToWrite);
           ++channel) {
        ringBuffer.copyFrom(static_cast<int>(channel),
                            writePosition,
                            source,
                            static_cast<int>(channel),
                            start1,
                            section1size);
      }
      // Block 1 Section 2
      int section2size = size1 - section1size;
      if (section2size > 0) {
        for (size_t channel = 0; channel < static_cast<size_t>(channelsToWrite);
             ++channel) {
          ringBuffer.copyFrom(static_cast<int>(channel),
                              0,
                              source,
                              static_cast<int>(channel),
                              start1 + section1size,
                              section2size);
        }
      }
    }
    // Block 2 Section 3
    if (size2 > 0) {
      int block2start = (writePosition + size1) % bufferSize;
      int section3size = jmin(size2, bufferSize - block2start);
      for (size_t channel = 0; channel < static_cast<size_t>(channelsToWrite);
           ++channel) {
        ringBuffer.copyFrom(static_cast<int>(channel),
                            block2start,
                            source,
                            static_cast<int>(channel),
                            start2,
                            section3size);
      }
      // Block 2 Section 4
      int section4size = size2 - section3size;
      if (section4size > 0) {
        for (size_t channel = 0; channel < static_cast<size_t>(channelsToWrite);
             ++channel) {
          ringBuffer.copyFrom(static_cast<int>(channel),
                              0,
                              source,
                              static_cast<int>(channel),
                              start2 + section3size,
                              section4size);
        }
      }
    }

    updateWritePosition(samplesToWrite);
    _bufferToWrite.finishedRead(size1 + size2);
  }

  //============================================================================
  /**
   * @brief Resizes the ring buffer.
   *
   * @param _numChannelsToAllocate The new number of channels.
   * @param _numSamplesToAllocate The new buffer size.
   */
  forcedinline void resize(const int _numChannelsToAllocate,
                           const int _numSamplesToAllocate) noexcept
  {
    ringBuffer.setSize(_numChannelsToAllocate, _numSamplesToAllocate);
  }

  //============================================================================
  /**
   * @brief Gets the number of channels in the ring buffer.
   *
   * @return The number of channels.
   */
  forcedinline int getNumChannels() const noexcept
  {
    return ringBuffer.getNumChannels();
  }

  //============================================================================
  /**
   * @brief Gets the number of samples in the ring buffer.
   *
   * @return The number of samples.
   */
  forcedinline int getNumSamples() const noexcept
  {
    return ringBuffer.getNumSamples();
  }

  //============================================================================
  /**
   * @brief Gets the current write position in the ring buffer.
   *
   * @return The write position.
   */
  forcedinline int getWritePosition() const noexcept { return writePosition; }

  //============================================================================
  /**
   * @brief Clears the ring buffer.
   */
  forcedinline void clear() noexcept
  {
    ringBuffer.clear();
    writePosition = 0;
  }

  //============================================================================
  /**
   * @brief Gets the underlying audio buffer.
   *
   * @return A reference to the audio buffer.
   */
  forcedinline AudioBuffer& getBuffer() noexcept { return ringBuffer; }

protected:
  //============================================================================
  /**
   * @brief Updates the write position in the ring buffer.
   *
   * @param _increment The amount to increment the write position.
   */
  forcedinline void updateWritePosition(const int _increment) noexcept
  {
    bool moveWriteOverRead = false;
    int newWritePosition = (writePosition + _increment) % getNumSamples();
    if (writePosition == readPositions[0]) [[unlikely]] {
      writePosition = newWritePosition;
      return;
    }

    // Check if the new write position overlaps with any read positions
    for (size_t channel = 0; channel < static_cast<size_t>(getNumChannels());
         ++channel) {
      for (size_t i = 0; i < static_cast<size_t>(_increment); ++i) {
        if ((writePosition + static_cast<int>(i)) % getNumSamples() ==
            readPositions[channel]) [[unlikely]] {
          moveWriteOverRead = true;
          break;
        }
      }
    }
    // Update write position
    writePosition = newWritePosition;

    // Update read positions if necessary
    if (moveWriteOverRead) {
      for (size_t channel = 0; channel < static_cast<size_t>(getNumChannels());
           ++channel) {
        readPositions[channel] = writePosition;
      }
    }
  }

private:
  AudioBuffer ringBuffer;
  int writePosition;
  std::vector<int> readPositions;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RingAudioBuffer)
};

} // namespace data
} // namespace dsp
} // namespace dmt