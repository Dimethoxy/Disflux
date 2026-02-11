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
};

} // namespace widget
} // namespace gui
} // namespace dmt
