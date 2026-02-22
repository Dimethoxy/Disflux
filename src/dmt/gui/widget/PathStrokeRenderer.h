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
   * The path is stroked with beveled joints and rounded end caps for a clean
   * waveform appearance.
   */
  inline void draw(juce::Graphics& _graphics,
                   RingBuffer& _ringBuffer,
                   int _channel,
                   const RenderContext& _context) override
  {
    this->currentX = _context.drawStartX;
    const auto path = buildPath(_ringBuffer, _channel, _context);
    this->strokePath(_graphics, path, _context);
  }

  //============================================================================
private:
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
    path.startNewSubPath(this->currentX,
                         this->sampleToY(this->currentSample,
                                         _context.halfHeight,
                                         _context.amplitude));

    for (size_t i = 0; i < static_cast<size_t>(_context.sampleCount); ++i) {
      const int sampleIndex = _context.firstSampleIndex + static_cast<int>(i);
      this->currentSample = _ringBuffer.getSample(_channel, sampleIndex);
      this->currentX += _context.pixelsPerSample;
      path.lineTo(this->currentX,
                  this->sampleToY(this->currentSample,
                                  _context.halfHeight,
                                  _context.amplitude));
    }

    return path;
  }
};

} // namespace widget
} // namespace gui
} // namespace dmt
