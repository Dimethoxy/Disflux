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
 * This renderer builds a continuous JUCE Path from the audio samples and
 * strokes it with a configurable thickness. It produces high-quality,
 * anti-aliased waveform visuals at the cost of higher CPU usage compared to
 * simpler rendering strategies.
 *
 * The renderer maintains persistent state (currentX and currentSample) between
 * frames to ensure visual continuity of the waveform across render calls.
 * Sub-pixel positioning is preserved to avoid visual jitter.
 *
 * Samples are read directly from the ring buffer — no data is copied.
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
    // Maintain sub-pixel continuity across frames
    currentX = currentX - static_cast<int>(currentX) + _context.drawStartX;

    const float startY =
      static_cast<float>(_context.halfHeight) +
      currentSample * _context.halfHeight * _context.amplitude;
    const auto startPoint = juce::Point<float>(currentX, startY);

    juce::Path path;
    path.startNewSubPath(startPoint);

    for (size_t i = 0; i < static_cast<size_t>(_context.sampleCount); ++i) {
      const int sampleIndex = _context.firstSampleIndex + static_cast<int>(i);
      currentSample = _ringBuffer.getSample(_channel, sampleIndex);
      currentX += _context.pixelsPerSample;
      const float y = static_cast<float>(_context.halfHeight) +
                      currentSample * _context.halfHeight * _context.amplitude;
      const auto point = juce::Point<float>(currentX, y);
      path.lineTo(point);
    }

    juce::PathStrokeType strokeType(_context.thickness * _context.sizeFactor,
                                    juce::PathStrokeType::JointStyle::mitered,
                                    juce::PathStrokeType::EndCapStyle::square);

    _graphics.setColour(juce::Colours::white);
    _graphics.strokePath(path, strokeType);
  }

  //============================================================================
private:
  //============================================================================
  /** Last sample value for waveform continuity between frames. */
  SampleType currentSample = static_cast<SampleType>(0.0f);

  /** Current X position with sub-pixel precision for visual continuity. */
  float currentX = 0.0f;
};

} // namespace widget
} // namespace gui
} // namespace dmt
