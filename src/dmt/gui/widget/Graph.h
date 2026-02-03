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
 * Graph widget for visualizing sample data in real-time. Designed for
 * performance and type safety, this component renders a waveform-like
 * visualization using a user-supplied data source.
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
 * @brief Graph widget for visualizing sample data in real-time.
 *
 * @tparam SampleType The numeric type of the samples to be visualized.
 *
 * @details
 * This component draws a waveform-like graph using a user-supplied data source.
 * The data source is a callable that returns a sample value for a given index.
 * Designed for real-time performance and type safety, this widget is intended
 * for use in audio and signal processing applications where efficient rendering
 * is critical. The rendering is performed in the paint() method, which is
 * optimized for minimal allocations and maximum throughput.
 */
template<typename SampleType>
class Graph : public juce::Component
{
  //==============================================================================
  using DataSource = std::function<SampleType(int32_t)>;

public:
  //==============================================================================
  /**
   * @brief Constructs a Graph widget with the given data source.
   *
   * @param _dataSource A callable that returns a sample value for a given
   * index.
   *
   * @details
   * The data source should be a fast, thread-safe callable suitable for
   * real-time use. The constructor is constexpr for maximum compile-time
   * optimization and noexcept for exception safety.
   */
  constexpr explicit inline Graph(DataSource _dataSource) noexcept
    : dataSource(_dataSource)
  {
  }

  //==============================================================================
  /**
   * @brief Handles component resizing. Intentionally left empty.
   *
   * @details
   * This override is required by JUCE. All layout is handled in paint().
   */
  inline void resized() override {}

  //==============================================================================
  /**
   * @brief Paints the graph visualization.
   *
   * @param _g The graphics context.
   *
   * @details
   * This method constructs a JUCE Path representing the waveform and renders
   * it. The path is preallocated for performance. Explicit casts are used for
   * type safety. The method is noexcept to guarantee real-time safety.
   */
  inline void paint(juce::Graphics& _g) noexcept override
  {
    const int32_t width = getWidth();
    const int32_t height = getHeight();

    juce::Path path;
    path.preallocateSpace(static_cast<size_t>((3 * width) + 6));
    path.startNewSubPath(0.0f, static_cast<float>(height) / 2.0f);

    for (size_t i = 0; i < static_cast<size_t>(width); ++i) {
      const SampleType sample = dataSource(static_cast<int32_t>(i));
      path.lineTo(static_cast<float>(i),
                  static_cast<float>(height) / 2.0f -
                    sample * static_cast<float>(height) / 2.0f);
    }

    path.lineTo(static_cast<float>(width), static_cast<float>(height) / 2.0f);
    path.closeSubPath();

    _g.setColour(juce::Colours::white);
    _g.strokePath(path, juce::PathStrokeType(3.0f));
  }

  //==============================================================================

private:
  //==============================================================================
  // Members initialized in the initializer list
  DataSource dataSource;

  //==============================================================================
  // Other members

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Graph)
};

} // namespace widget
} // namespace gui
} // namespace dmt