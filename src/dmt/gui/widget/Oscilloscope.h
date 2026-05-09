//==============================================================================
/* ██████╗ ██╗███╗   ███╗███████╗████████╗██╗  ██╗ ██████╗ ██╗  ██╗██╗   ██╗
 * ██╔══██╗██║████╗ ████║██╔════╝╚══██╔══╝██║  ██║██╔═══██╗╚██╗██╔╝╚██╗ ██╔╝
 * ██║  ██║██║██╔████╔██║█████╗     ██║   ███████║██║   ██║ ╚███╔╝  ╚████╔╝
 * ██║  ██║██║██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██║   ██║ ██╔██╗   ╚██╔╝
 * ██████╔╝██║██║ ╚═╝ ██║███████╗   ██║   ██║  ██║╚██████╔╝██╔╝ ██╗   ██║
 * ╚═════╝ ╚═╝╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * Part of the Dimethoxy Library, primarily intended for Dimethoxy plugins.
 * External use is permitted but not recommended.
 * No support or compatibility guarantees are provided.
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

#include "gui/widget/MinMaxRenderer.h"
#include "gui/widget/PathStrokeRenderer.h"
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
 * minimal overhead using lock-free double buffering for image access.
 *
 * The oscilloscope is intended to be used with a lock-free ring buffer for
 * audio data, and supports customization of amplitude, thickness, and
 * samples-per-pixel for flexible display scaling.
 *
 * The rendering thread is started upon construction and stopped on destruction.
 * The image is updated periodically, and can be retrieved via getFrontImage().
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
  using Settings = dmt::Settings;
  using Renderer = OscilloscopeRenderer<SampleType>;

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
    , renderer(std::make_shared<MinMaxRenderer<SampleType>>())
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
   * @brief Retrieves the current front image for GUI rendering.
   *
   * @return A reference to the front buffer image.
   *
   * @details
   * The render thread only writes to the back buffer, so the front image is
   * safe for concurrent GUI reads.
   */
  [[nodiscard]] inline const juce::Image& getFrontImage() const noexcept
  {
    const int frontIndex = frontBufferIndex.load(std::memory_order_acquire);
    return images[static_cast<size_t>(frontIndex)];
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
    bounds = _newBounds;

    renderWidth.store(_newBounds.getWidth(), std::memory_order_relaxed);
    renderHeight.store(_newBounds.getHeight(), std::memory_order_relaxed);
    resizePending.store(true, std::memory_order_release);
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
    rawSamplesPerPixel.store(_newRawSamplesPerPixel, std::memory_order_relaxed);
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
    amplitude.store(_newAmplitude, std::memory_order_relaxed);
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
    thickness.store(_newThickness, std::memory_order_relaxed);
  }

  //==============================================================================
  /**
   * @brief Sets the rendering strategy for the oscilloscope.
   *
   * @param _newRenderer A unique pointer to the new renderer implementation.
   *
   * @details
   * Swaps the current rendering strategy atomically to avoid contention with
   * the rendering thread. The previous renderer is destroyed when the new one
   * is set.
   */
  inline void setRenderer(std::unique_ptr<Renderer> _newRenderer)
  {
    std::atomic_store_explicit(
      &renderer,
      std::shared_ptr<Renderer>(std::move(_newRenderer)),
      std::memory_order_release);
  }

  //==============================================================================
protected:
  //==============================================================================
  /**
   * @brief Main rendering thread loop.
   *
   * @details
   * Periodically updates the oscilloscope image by rendering the latest audio
   * samples. Uses a back buffer to avoid GUI contention.
   * The wait interval is set high to minimize CPU usage; rendering is not
   * continuous but event-driven.
   */
  inline void run() override
  {
    while (!threadShouldExit()) {
      wait(10000);

      if (resizePending.exchange(false, std::memory_order_acq_rel)) {
        resizeImage(renderWidth.load(std::memory_order_relaxed),
                    renderHeight.load(std::memory_order_relaxed));
      }

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

    // Avoid illegal sizes
    if (_width <= 0 || _height <= 0) {
      return;
    }

    for (auto& image : images) {
      image = Image(PixelFormat::ARGB, _width + 10, _height, true);
    }

    frontBufferIndex.store(0, std::memory_order_release);
    subPixelOffset = 0.0f;

    for (auto& image : images) {
      juce::Graphics imageGraphics(image);
      imageGraphics.setColour(juce::Colours::white);
      imageGraphics.drawLine(0,
                             static_cast<float>(_height) / 2.0f,
                             static_cast<float>(_width + 10),
                             static_cast<float>(_height) / 2.0f,
                             3.0f);
    }
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

    const int width = renderWidth.load(std::memory_order_relaxed);
    const int height = renderHeight.load(std::memory_order_relaxed);

    if (width <= 0 || height <= 0)
      return;

    const int halfHeight = height / 2;

    const float samplesPerPixel =
      rawSamplesPerPixel.load(std::memory_order_relaxed) * size;

    if (samplesPerPixel <= 0.0f)
      return;

    const int bufferSize = ringBuffer.getNumSamples();
    const int readPosition = ringBuffer.getReadPosition(channel);
    const int samplesToRead = bufferSize - readPosition;

    const int maxSamplesToDraw =
      (int)std::floor(samplesPerPixel * (float)width);

    const int samplesToDraw = jmin(samplesToRead, maxSamplesToDraw);
    const int firstSamplesToDraw = readPosition;

    const float exactPixelsToDraw = (float)samplesToDraw / samplesPerPixel;

    const float totalShift = exactPixelsToDraw + subPixelOffset;
    const int pixelToDraw = (int)totalShift;

    if (pixelToDraw <= 0)
      return;

    ringBuffer.incrementReadPosition(channel, samplesToDraw);

    const int currentFront = frontBufferIndex.load(std::memory_order_acquire);
    const int backIndex = currentFront == 0 ? 1 : 0;

    auto& backImage = images[(size_t)backIndex];

    backImage = images[(size_t)currentFront].createCopy();

    // Left scrolling 2.0: Hopefully without crashing host DAWs
    const int shift = jmin(pixelToDraw, width);
    if (shift > 0) {
      // Move content LEFT safely inside bounds
      // backImage.moveImageSection(0,
      //                            0, // destX, destY
      //                            shift,
      //                            0, // sourceX, sourceY
      //                            width - shift,
      //                            height);

      backImage.clear(juce::Rectangle<int>(width - shift, 0, shift, height),
                      juce::Colours::transparentBlack);
    }

    // Render new audio data
    juce::Graphics g(backImage);

    const typename Renderer::RenderContext context{
      firstSamplesToDraw,
      samplesToDraw,
      (float)(width - shift) + subPixelOffset,
      1.0f / samplesPerPixel,
      halfHeight,
      amplitude.load(std::memory_order_relaxed),
      thickness.load(std::memory_order_relaxed),
      size
    };

    subPixelOffset = totalShift - (float)pixelToDraw;

    if (auto currentRenderer =
          std::atomic_load_explicit(&renderer, std::memory_order_acquire)) {
      currentRenderer->draw(g, ringBuffer, channel, context);
    }

    frontBufferIndex.store(backIndex, std::memory_order_release);
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
  std::array<Image, 2> images = { Image(PixelFormat::ARGB, 1, 1, true),
                                  Image(PixelFormat::ARGB, 1, 1, true) };
  std::atomic<int> frontBufferIndex{ 0 };
  std::atomic<int> renderWidth{ 1 };
  std::atomic<int> renderHeight{ 1 };
  std::atomic<bool> resizePending{ true };

  std::shared_ptr<Renderer> renderer;
  float subPixelOffset = 0.0f;
  std::atomic<float> rawSamplesPerPixel{ 10.0f };
  std::atomic<float> amplitude{ 1.0f };
  std::atomic<float> thickness{ 3.0f };
  const float& size;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscilloscope)
};

} // namespace widget
} // namespace gui
} // namespace dmt