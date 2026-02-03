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
 * Real-time oscilloscope widget for visualizing audio buffers with minimal
 * latency and maximum performance. Designed for use in JUCE-based GUIs.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================
/**
 * @brief Real-time oscilloscope widget for audio visualization.
 *
 * @tparam SampleType The sample type (e.g., float, double) used for audio data.
 *
 * @details
 * This class provides a high-performance oscilloscope visualization for audio
 * buffers, optimized for real-time use in GUI applications. It leverages a
 * background thread to render the waveform into a JUCE image, which can be
 * efficiently displayed in the GUI. The design ensures thread safety and
 * minimal locking overhead, using a read-write lock for image access.
 *
 * The oscilloscope is intended to be used with a lock-free ring buffer for
 * audio data, and supports customization of amplitude, thickness, and
 * samples-per-pixel for flexible display scaling.
 *
 * The rendering thread is started upon construction and stopped on destruction.
 * The image is updated periodically, and can be retrieved via getImage().
 */
template<typename SampleType>
class alignas(64) Oscilloscope : public juce::Thread
{
  //==============================================================================
public:
  using RingBuffer = dmt::dsp::data::RingAudioBuffer<SampleType>;
  using Image = juce::Image;
  using Graphics = juce::Graphics;
  using String = juce::String;
  using Thread = juce::Thread;
  using PixelFormat = juce::Image::PixelFormat;
  using ReadWriteLock = juce::ReadWriteLock;
  using Settings = dmt::Settings;

  //==============================================================================
  /**
   * @brief Constructs the oscilloscope and starts the rendering thread.
   *
   * @param _ringBuffer Reference to the ring buffer containing audio samples.
   * @param _channel The audio channel to visualize.
   *
   * @details
   * The rendering thread is started immediately. The oscilloscope will read
   * from the provided ring buffer and visualize the specified channel.
   */
  explicit Oscilloscope(RingBuffer& _ringBuffer,
                        const int32_t _channel,
                        const float& _sizeFactor) noexcept
    : Thread(String("Oscilloscope" + juce::String(_channel)))
    , ringBuffer(_ringBuffer)
    , channel(_channel)
    , size(_sizeFactor)
  {
    startThread();
  }

  //==============================================================================
  /**
   * @brief Destructor. Stops the rendering thread.
   *
   * @details
   * Ensures the rendering thread is stopped gracefully before destruction.
   */
  inline ~Oscilloscope() noexcept override { stopThread(1000); }

  //==============================================================================
  /**
   * @brief Retrieves a copy of the current oscilloscope image.
   *
   * @return A copy of the rendered JUCE image.
   *
   * @details
   * The returned image is thread-safe and can be used in the GUI.
   */
  [[nodiscard]] inline juce::Image getImage() const
  {
    const ScopedReadLock readLock(imageLock);
    return image.createCopy();
  }

  //==============================================================================
  /**
   * @brief Sets the bounds for the oscilloscope image and resizes accordingly.
   *
   * @param _newBounds The new bounds rectangle.
   *
   * @details
   * Resizes the internal image buffer and updates the drawing area.
   */
  inline void setBounds(juce::Rectangle<int> _newBounds)
  {
    resizeImage(_newBounds.getWidth(), _newBounds.getHeight());
    bounds = _newBounds;
  }

  //==============================================================================
  /**
   * @brief Gets the current bounds of the oscilloscope image.
   *
   * @return The bounds rectangle.
   */
  [[nodiscard]] inline juce::Rectangle<int> getBounds() const noexcept
  {
    return bounds;
  }

  //==============================================================================
  /**
   * @brief Sets the number of raw samples per pixel for display scaling.
   *
   * @param _newRawSamplesPerPixel The new samples-per-pixel value.
   *
   * @details
   * Adjusts the horizontal scaling of the waveform.
   */
  inline void setRawSamplesPerPixel(float _newRawSamplesPerPixel) noexcept
  {
    rawSamplesPerPixel = _newRawSamplesPerPixel;
  }

  //==============================================================================
  /**
   * @brief Sets the amplitude scaling factor for the waveform.
   *
   * @param _newAmplitude The new amplitude scaling factor.
   *
   * @details
   * Controls the vertical scaling of the waveform.
   */
  inline void setAmplitude(float _newAmplitude) noexcept
  {
    amplitude = _newAmplitude;
  }

  //==============================================================================
  /**
   * @brief Sets the thickness of the waveform stroke.
   *
   * @param _newThickness The new stroke thickness.
   *
   * @details
   * Controls the visual thickness of the waveform path.
   */
  inline void setThickness(float _newThickness) noexcept
  {
    thickness = _newThickness;
  }

  //==============================================================================
protected:
  //==============================================================================
  /**
   * @brief Main rendering thread loop.
   *
   * @details
   * Periodically updates the oscilloscope image by rendering the latest audio
   * samples. Uses a write lock to ensure exclusive access to the image.
   * The wait interval is set high to minimize CPU usage; rendering is not
   * continuous but event-driven.
   */
  inline void run() override
  {
    while (!threadShouldExit()) {
      wait(10000);
      const ScopedWriteLock writeLock(imageLock);
      render();
    }
  }

  //==============================================================================
  /**
   * @brief Resizes the internal image buffer and draws the midline.
   *
   * @param _width The new image width.
   * @param _height The new image height.
   *
   * @details
   * The image is resized and a horizontal midline is drawn for reference.
   */
  inline void resizeImage(const int _width, const int _height)
  {
    TRACER("Oscilloscope::resizeImage");
    const ScopedWriteLock writeLock(imageLock);

    // Avoid illegal sizes
    if (_width <= 0 || _height <= 0) {
      return;
    }

    image = Image(PixelFormat::ARGB, _width + 10, _height, true);

    juce::Graphics imageGraphics(image);
    imageGraphics.setColour(juce::Colours::white);
    imageGraphics.drawLine(0,
                           static_cast<float>(_height) / 2.0f,
                           static_cast<float>(_width + 10),
                           static_cast<float>(_height) / 2.0f,
                           3.0f);
  }

  //==============================================================================
  /**
   * @brief Renders the oscilloscope waveform into the image.
   *
   * @details
   * Reads samples from the ring buffer, updates the image by moving the
   * existing content, clears the new region, and draws the waveform path.
   * Uses explicit type conversions for safety and clarity.
   */
  inline void render()
  {
    TRACER("Oscilloscope::render");
    const int width = bounds.getWidth();
    const int height = bounds.getHeight();
    const int halfHeight = height / 2;
    float samplesPerPixel = rawSamplesPerPixel * size;

    const int bufferSize = ringBuffer.getNumSamples();
    const int readPosition = ringBuffer.getReadPosition(channel);
    const int samplesToRead = bufferSize - readPosition;

    const int maxSamplesToDraw =
      static_cast<int>(std::floor(samplesPerPixel * static_cast<float>(width)));
    const int samplesToDraw = jmin(samplesToRead, maxSamplesToDraw);
    const int firstSamplesToDraw = readPosition;

    const int pixelToDraw = static_cast<int>(samplesToDraw / samplesPerPixel);
    ringBuffer.incrementReadPosition(channel, samplesToDraw);

    // Image move
    const int destX = 0 - pixelToDraw;
    image.moveImageSection(destX,      // destX
                           0,          // destY
                           0,          // srcX
                           0,          // srcY
                           width + 10, // width
                           height);    // height

    // Clear the new part of the image
    juce::Rectangle<int> clearRect(
      width - pixelToDraw + 10, 0, pixelToDraw, height);
    image.clear(clearRect, juce::Colours::transparentBlack);

    // Generate path for new samples
    currentX = currentX - static_cast<int>(currentX) + width - pixelToDraw;
    float pixelsPerSample = 1.0f / samplesPerPixel;

    const float startY = halfHeight + currentSample * halfHeight * amplitude;
    const auto startPoint = juce::Point<float>(currentX, startY);

    juce::Path path;
    path.startNewSubPath(startPoint);

    for (size_t i = 0; i < static_cast<size_t>(samplesToDraw); ++i) {
      const int sampleIndex = firstSamplesToDraw + static_cast<int>(i);
      currentSample = ringBuffer.getSample(channel, sampleIndex);
      currentX += pixelsPerSample;
      const float y = halfHeight + currentSample * halfHeight * amplitude;
      const auto point = juce::Point<float>(currentX, y);
      path.lineTo(point);
    }

    juce::PathStrokeType strokeType(thickness * size,
                                    juce::PathStrokeType::JointStyle::mitered,
                                    juce::PathStrokeType::EndCapStyle::square);

    juce::Graphics imageGraphics(image);
    imageGraphics.setColour(juce::Colours::white);
    imageGraphics.strokePath(path, strokeType);
  }

  //==============================================================================
private:
  //==============================================================================
  // Members initialized in the initializer list
  RingBuffer& ringBuffer;
  const int32_t channel;

  //==============================================================================
  // Other members
  juce::Rectangle<int> bounds = juce::Rectangle<int>(0, 0, 1, 1);
  Image image = Image(PixelFormat::ARGB, 1, 1, true);
  ReadWriteLock imageLock;

  SampleType currentSample = static_cast<SampleType>(0.0f);
  float currentX = 0.0f;
  float rawSamplesPerPixel = 10.0f;
  float amplitude = 1.0f;
  float thickness = 3.0f;
  const float& size;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscilloscope)
};

} // namespace widget
} // namespace gui
} // namespace dmt