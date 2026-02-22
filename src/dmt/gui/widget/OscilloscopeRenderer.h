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
 * Abstract base class for oscilloscope rendering strategies. Defines the
 * interface for drawing waveform data onto a JUCE Graphics context. Concrete
 * implementations provide different visualization algorithms while the
 * Oscilloscope class handles image management, scrolling, and threading.
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
 * @brief Abstract base class for oscilloscope rendering strategies.
 *
 * @tparam SampleType The sample type (e.g., float, double) used for audio data.
 *
 * @details
 * This class defines the interface for rendering waveform data onto a JUCE
 * Graphics context. Concrete implementations provide different visualization
 * algorithms (e.g., path stroking, point plotting, filled waveforms).
 *
 * The renderer reads sample data directly from the ring buffer by reference,
 * avoiding any additional allocations or data copies. The Oscilloscope class
 * handles all image management (scrolling, clearing) and passes a
 * pre-configured Graphics context along with a RenderContext struct containing
 * the parameters needed for drawing.
 *
 * Renderers may maintain their own persistent state between frames (e.g.,
 * sub-pixel position tracking) to ensure visual continuity.
 *
 * Common helper functions for coordinate conversion and path stroking are
 * provided here to avoid duplication across renderer implementations.
 */
template<typename SampleType>
class OscilloscopeRenderer
{
  //============================================================================
public:
  using RingBuffer = dmt::dsp::data::RingAudioBuffer<SampleType>;

  //============================================================================
  /**
   * @brief Parameters passed from the Oscilloscope to the renderer each frame.
   *
   * @details
   * This struct contains all pre-computed values the renderer needs to draw
   * the new waveform segment. The Oscilloscope computes these from its own
   * state and the ring buffer, then passes them to the renderer. This avoids
   * the renderer needing to know about image scrolling or buffer management.
   */
  struct RenderContext
  {
    /** Index of the first sample to read from the ring buffer. */
    int firstSampleIndex;

    /** Number of samples to draw in this frame. */
    int sampleCount;

    /** X coordinate where the new drawing region starts on the image. */
    float drawStartX;

    /** Horizontal spacing: pixels per sample. */
    float pixelsPerSample;

    /** Vertical center of the drawing area in pixels. */
    int halfHeight;

    /** Amplitude scaling factor for the waveform. */
    float amplitude;

    /** Stroke thickness for the waveform. */
    float thickness;

    /** Global size scaling factor. */
    float sizeFactor;
  };

  //============================================================================
  /**
   * @brief Virtual destructor for safe polymorphic deletion.
   */
  virtual ~OscilloscopeRenderer() = default;

  //============================================================================
  /**
   * @brief Draws a waveform segment onto the provided Graphics context.
   *
   * @param _graphics The JUCE Graphics context targeting the oscilloscope
   *                  image. Already configured for the current frame.
   * @param _ringBuffer Reference to the ring buffer containing audio samples.
   *                    Read directly — no data is copied.
   * @param _channel The audio channel index to read from.
   * @param _context Pre-computed rendering parameters for this frame.
   *
   * @details
   * Implementations should read samples directly from the ring buffer using
   * the indices provided in the context. The Graphics context is already
   * attached to the oscilloscope image, and the image has already been
   * scrolled and cleared by the Oscilloscope class.
   */
  virtual void draw(juce::Graphics& _graphics,
                    RingBuffer& _ringBuffer,
                    int _channel,
                    const RenderContext& _context) = 0;

  //============================================================================
protected:
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
   * @brief Strokes the given path onto the graphics context.
   *
   * @param _graphics The JUCE Graphics context targeting the oscilloscope
   *                  image.
   * @param _path The waveform path to stroke.
   * @param _context Pre-computed rendering parameters for this frame.
   *
   * @details
   * Configures the stroke type with beveled joints and rounded end caps,
   * and uses a solid white colour for the stroke.
   */
  inline void strokePath(juce::Graphics& _graphics,
                         const juce::Path& _path,
                         const RenderContext& _context) const
  {
    juce::PathStrokeType strokeType(_context.thickness * _context.sizeFactor,
                                    juce::PathStrokeType::JointStyle::beveled,
                                    juce::PathStrokeType::EndCapStyle::rounded);
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
