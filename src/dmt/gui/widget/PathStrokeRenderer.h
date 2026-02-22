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
 * Path stroke oscilloscope renderer. Draws a continuous waveform path using
 * JUCE's Path and PathStrokeType for high-quality, anti-aliased rendering.
 * This is the original (and most expensive) rendering algorithm.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/widget/OscilloscopeRenderer.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================
/**
 * @brief Path stroke oscilloscope renderer for high-quality waveform drawing.
 *
 * @tparam SampleType The sample type (e.g., float, double) used for audio data.
 *
 * @details
 * This is a very simple renderer that draws a path with one point for each
 * sample. It's non-hardware-accelerated, and produces high-quality visuals at
 * the cost of increased CPU usage.
 *
 * The renderer maintains persistent state (currentX and currentSample) between
 * frames to ensure visual continuity of the waveform across render calls.
 * Sub-pixel positioning is preserved to avoid visual jitter.
 *
 * Samples are read directly from the ring buffer, no data is copied.
 */
template<typename SampleType>
class PathStrokeRenderer : public OscilloscopeRenderer<SampleType>
{
  //============================================================================
public:
  using RingBuffer = typename OscilloscopeRenderer<SampleType>::RingBuffer;
  using RenderContext =
    typename OscilloscopeRenderer<SampleType>::RenderContext;

  //============================================================================
  /**
   * @brief Draws a waveform segment using path stroking.
   *
   * @param _graphics The JUCE Graphics context targeting the oscilloscope
   *                  image.
   * @param _ringBuffer Reference to the ring buffer containing audio samples.
   * @param _channel The audio channel index to read from.
   * @param _context Pre-computed rendering parameters for this frame.
   *
   * @details
   * Builds a continuous path from the provided samples, maintaining sub-pixel
   * continuity with the previous frame via the persistent currentX state.
   * The path is stroked with mitered joints and square end caps for a clean
   * waveform appearance.
   */
  inline void draw(juce::Graphics& _graphics,
                   RingBuffer& _ringBuffer,
                   int _channel,
                   const RenderContext& _context) override
  {
    currentX = _context.drawStartX;
    const auto path = buildPath(_ringBuffer, _channel, _context);
    strokePath(_graphics, path, _context);
  }

  //============================================================================
private:
  //============================================================================
  /**
   * @brief Converts a sample value to a Y pixel coordinate.
   *
   * @param _sample The sample value to convert.
   * @param _halfHeight The vertical center of the drawing area in pixels.
   * @param _amplitude The amplitude scaling factor.
   *
   * @return The Y coordinate in pixels.
   */
  [[nodiscard]] inline float sampleToY(SampleType _sample,
                                       int _halfHeight,
                                       float _amplitude) const noexcept
  {
    return static_cast<float>(_halfHeight) + _sample * _halfHeight * _amplitude;
  }

  //============================================================================
  /**
   * @brief Builds a continuous path from the ring buffer samples.
   *
   * @param _ringBuffer Reference to the ring buffer containing audio samples.
   * @param _channel The audio channel index to read from.
   * @param _context Pre-computed rendering parameters for this frame.
   *
   * @return The constructed JUCE Path representing the waveform segment.
   *
   * @details
   * Reads samples directly from the ring buffer and advances the persistent
   * currentX position to maintain sub-pixel continuity between frames.
   */
  [[nodiscard]] inline juce::Path buildPath(RingBuffer& _ringBuffer,
                                            int _channel,
                                            const RenderContext& _context)
  {
    juce::Path path;
    path.startNewSubPath(
      currentX,
      sampleToY(currentSample, _context.halfHeight, _context.amplitude));

    for (size_t i = 0; i < static_cast<size_t>(_context.sampleCount); ++i) {
      const int sampleIndex = _context.firstSampleIndex + static_cast<int>(i);
      currentSample = _ringBuffer.getSample(_channel, sampleIndex);
      currentX += _context.pixelsPerSample;
      path.lineTo(
        currentX,
        sampleToY(currentSample, _context.halfHeight, _context.amplitude));
    }

    return path;
  }

  //============================================================================
  /**
   * @brief Strokes the given path onto the graphics context.
   *
   * @param _graphics The JUCE Graphics context targeting the oscilloscope
   *                  image.
   * @param _path The waveform path to stroke.
   * @param _context Pre-computed rendering parameters for this frame.
   *
   * @details
   * Configures the stroke type with mitered joints and square end caps,
   * then paints the path in white.
   */
  inline void strokePath(juce::Graphics& _graphics,
                         const juce::Path& _path,
                         const RenderContext& _context) const
  {
    juce::PathStrokeType strokeType(_context.thickness * _context.sizeFactor,
                                    juce::PathStrokeType::JointStyle::mitered,
                                    juce::PathStrokeType::EndCapStyle::square);

    // Generate random color for testing
    // juce::Random random;
    // float red = random.nextFloat();
    // float green = random.nextFloat();
    // float blue = random.nextFloat();
    // juce::Colour randomColour =
    //   juce::Colour::fromFloatRGBA(red, green, blue, 1.0f);
    // _graphics.setColour(randomColour);

    _graphics.setColour(juce::Colours::white);
    _graphics.strokePath(_path, strokeType);
  }

  //============================================================================
  /** Last sample value for waveform continuity between frames. */
  SampleType currentSample = static_cast<SampleType>(0.0f);

  /** Current X position with sub-pixel precision for visual continuity. */
  float currentX = 0.0f;
};

} // namespace widget
} // namespace gui
} // namespace dmt
