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
 * Min/Max binned oscilloscope renderer optimized for high frame rates. Bins
 * samples to pixel columns, drawing only the minimum and maximum sample per
 * bin while preserving temporal order. This dramatically reduces path
 * complexity compared to per-sample rendering, trading visual fidelity for
 * significantly lower CPU usage at high sample densities.
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
 * @brief Min/Max binned oscilloscope renderer for high-framerate waveform
 *        drawing.
 *
 * @tparam SampleType The sample type (e.g., float, double) used for audio data.
 *
 * @details
 * Instead of drawing one point per sample like PathStrokeRenderer, this
 * renderer bins samples into pixel columns and draws only two points per bin:
 * the minimum and maximum sample values. The temporal order of min and max
 * within each bin is tracked, so the path correctly represents whether the
 * waveform went up or down first in that pixel column.
 *
 * This produces at most 2 path points per pixel column instead of potentially
 * many points per pixel, resulting in a much simpler path that is far cheaper
 * to stroke. The visual result is nearly identical to per-sample rendering at
 * typical zoom levels where multiple samples map to a single pixel.
 *
 * The renderer maintains persistent state (currentX and currentSample) between
 * frames to ensure visual continuity of the waveform across render calls.
 */
template<typename SampleType>
class MinMaxRenderer : public OscilloscopeRenderer<SampleType>
{
  //============================================================================
public:
  using RingBuffer = typename OscilloscopeRenderer<SampleType>::RingBuffer;
  using RenderContext =
    typename OscilloscopeRenderer<SampleType>::RenderContext;

  //============================================================================
  /**
   * @brief Draws a waveform segment using min/max binned path stroking.
   *
   * @param _graphics The JUCE Graphics context targeting the oscilloscope
   *                  image.
   * @param _ringBuffer Reference to the ring buffer containing audio samples.
   * @param _channel The audio channel index to read from.
   * @param _context Pre-computed rendering parameters for this frame.
   *
   * @details
   * Bins samples to pixel columns and builds a path through the min/max
   * extremes of each bin, preserving temporal order for accurate waveform
   * representation. The path is then stroked using the shared strokePath.
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
   * @brief Holds the min/max state for a single pixel column bin.
   *
   * @details
   * Tracks the minimum and maximum sample values encountered within a pixel
   * column, along with their sample indices to determine temporal ordering.
   */
  struct Bin
  {
    SampleType minSample = static_cast<SampleType>(0.0f);
    SampleType maxSample = static_cast<SampleType>(0.0f);
    size_t minIndex = 0;
    size_t maxIndex = 0;
    size_t count = 0;

    /** @brief Resets the bin with the first sample of a new pixel column. */
    inline void reset(SampleType _sample, size_t _index) noexcept
    {
      minSample = _sample;
      maxSample = _sample;
      minIndex = _index;
      maxIndex = _index;
      count = 1;
    }

    /** @brief Adds a sample to the bin, updating min/max as needed. */
    inline void addSample(SampleType _sample, size_t _index) noexcept
    {
      if (_sample < minSample) {
        minSample = _sample;
        minIndex = _index;
      }
      if (_sample > maxSample) {
        maxSample = _sample;
        maxIndex = _index;
      }
      ++count;
    }

    /** @brief Returns true if the minimum sample occurred before the max. */
    [[nodiscard]] inline bool minFirst() const noexcept
    {
      return minIndex <= maxIndex;
    }
  };

  //============================================================================
  /**
   * @brief Builds a min/max binned path from the ring buffer samples.
   *
   * @param _ringBuffer Reference to the ring buffer containing audio samples.
   * @param _channel The audio channel index to read from.
   * @param _context Pre-computed rendering parameters for this frame.
   *
   * @return The constructed JUCE Path representing the waveform segment.
   *
   * @details
   * Iterates through all samples, grouping them into pixel column bins.
   * For each completed bin, two points are added to the path in temporal
   * order (whichever extreme occurred first is drawn first). This preserves
   * the waveform's directional movement while reducing the point count to
   * at most two per pixel column.
   *
   * When only one sample falls in a bin, a single point is drawn.
   * The persistent currentX tracks the pixel column boundary for sub-pixel
   * continuity between frames.
   */
  [[nodiscard]] inline juce::Path buildPath(RingBuffer& _ringBuffer,
                                            int _channel,
                                            const RenderContext& _context)
  {
    juce::Path path;

    // Start the path from the last sample of the previous frame
    path.startNewSubPath(this->currentX,
                         this->sampleToY(this->currentSample,
                                         _context.halfHeight,
                                         _context.amplitude));

    const float samplesPerPixel = 1.0f / _context.pixelsPerSample;
    float nextBinBoundary = this->currentX + 1.0f;

    Bin bin;
    bool binActive = false;

    for (size_t i = 0; i < static_cast<size_t>(_context.sampleCount); ++i) {
      const int sampleIndex = _context.firstSampleIndex + static_cast<int>(i);
      const SampleType sample = _ringBuffer.getSample(_channel, sampleIndex);
      const float sampleX =
        this->currentX + static_cast<float>(i + 1) * _context.pixelsPerSample;

      if (!binActive) {
        bin.reset(sample, i);
        binActive = true;
      } else {
        bin.addSample(sample, i);
      }

      // Check if the next sample would cross into a new pixel column
      const float nextSampleX =
        this->currentX + static_cast<float>(i + 2) * _context.pixelsPerSample;
      const bool isLastSample =
        (i + 1 >= static_cast<size_t>(_context.sampleCount));
      const bool crossesBoundary = (nextSampleX >= nextBinBoundary);

      if (isLastSample || crossesBoundary) {
        // Finalize the current bin
        const float binX = std::min(sampleX, nextBinBoundary - 0.5f);

        if (bin.count == 1) {
          // Single sample in bin, draw one point
          path.lineTo(binX,
                      this->sampleToY(bin.minSample,
                                      _context.halfHeight,
                                      _context.amplitude));
        } else {
          // Multiple samples, draw min and max in temporal order
          if (bin.minFirst()) {
            path.lineTo(binX,
                        this->sampleToY(bin.minSample,
                                        _context.halfHeight,
                                        _context.amplitude));
            path.lineTo(binX,
                        this->sampleToY(bin.maxSample,
                                        _context.halfHeight,
                                        _context.amplitude));
          } else {
            path.lineTo(binX,
                        this->sampleToY(bin.maxSample,
                                        _context.halfHeight,
                                        _context.amplitude));
            path.lineTo(binX,
                        this->sampleToY(bin.minSample,
                                        _context.halfHeight,
                                        _context.amplitude));
          }
        }

        if (crossesBoundary) {
          nextBinBoundary = std::floor(nextSampleX) + 1.0f;
          binActive = false;
        }
      }
    }

    // Update persistent state for frame continuity
    const float totalAdvance =
      static_cast<float>(_context.sampleCount) * _context.pixelsPerSample;
    this->currentX += totalAdvance;
    if (_context.sampleCount > 0) {
      const int lastIndex =
        _context.firstSampleIndex + _context.sampleCount - 1;
      this->currentSample = _ringBuffer.getSample(_channel, lastIndex);
    }

    return path;
  }
};

} // namespace widget
} // namespace gui
} // namespace dmt
