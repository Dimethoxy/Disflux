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
 * Provides a lightweight, high-performance label widget for JUCE-based GUIs.
 * Designed for real-time audio applications where rendering efficiency and
 * type safety are critical. Supports both single-line and multi-line text,
 * with platform-specific font scaling for consistent appearance.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/utility/Scaleable.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================
/**
 * @brief A high-performance, type-safe label widget for JUCE GUIs.
 *
 * @details
 * This class provides a customizable label component optimized for real-time
 * audio applications. It supports both single-line and multi-line text
 * rendering, with platform-specific font scaling to ensure visual consistency
 * across operating systems. The design prioritizes rendering speed and type
 * safety, making it suitable for use in performance-critical GUI contexts.
 *
 * The label can be justified and colored as needed, and is intended to be used
 * as a lightweight alternative to more complex text components.
 */
class Label
  : public juce::Component
  , public dmt::Scaleable<Label>
{
  using String = juce::String;
  using Settings = dmt::Settings;
  using Justification = juce::Justification;

public:
  //==============================================================================
  /**
   * @brief Constructs a Label with the specified parameters.
   *
   * @param _text The text to display in the label.
   * @param _font The font to use for rendering the text.
   * @param _fontSize The base font size (will be scaled per platform).
   * @param _colour The colour to use for the text.
   * @param _justification The text justification (default: centredTop).
   * @param _multiline Whether to enable multi-line rendering (default: false).
   *
   * @details
   * The constructor is noexcept for maximum compile-time optimization and
   * real-time safety. Font scaling is handled internally for platform
   * consistency.
   */
  explicit Label(const String _text,
                 const juce::Font& _font,
                 const float& _fontSize,
                 const juce::Colour& _colour,
                 const Justification _justification = Justification::centredTop,
                 const bool _multiline = false) noexcept
    : juce::Component()
    , text(_text)
    , font(_font)
    , fontSize(_fontSize)
    , justification(_justification)
    , multiline(_multiline)
  {
    fontColour = &_colour;
  }

  //==============================================================================
  /**
   * @brief Paints the label's text and optional debug bounds.
   *
   * @param _g The graphics context for rendering.
   *
   * @details
   * This method applies platform-specific font scaling for consistent
   * appearance. If multi-line mode is enabled, text is rendered using
   * drawMultiLineText. Debug bounds are drawn if enabled in settings. The
   * function is marked noexcept for real-time safety.
   */
  inline void paint(juce::Graphics& _g) noexcept override
  {
    TRACER("Label::paint");
    auto bounds = getLocalBounds();

    // Set background colour
    if (backgroundColour != nullptr)
      _g.fillAll(*backgroundColour);

    // Draw bounds debug
    _g.setColour(juce::Colours::red);
    if (Settings::debugBounds)
      _g.drawRect(bounds, 1);

    // Set font size and scaling
    _g.setFont(font.withHeight(static_cast<float>(fontSize * size)));

    // Padding
    const float horizontalPadding = rawHorizontalPadding * size;

    // Draw text
    if (!multiline) {
      _g.setColour(*fontColour);
      _g.drawText(text,
                  bounds.reduced(static_cast<int>(horizontalPadding), 0),
                  justification,
                  true);
    } else {
      const int startX = bounds.getX() + static_cast<int>(horizontalPadding);
      const int baselineY =
        bounds.getY() + static_cast<int>(_g.getCurrentFont().getAscent());
      const int maximumLineWidth =
        bounds.getWidth() - static_cast<int>(2 * horizontalPadding);
      _g.setColour(*fontColour);
      _g.drawMultiLineText(
        text, startX, baselineY, maximumLineWidth, justification, 0.0f);
    }
  }

  //==============================================================================
  /**
   * @brief Sets the label's text.
   *
   * @param _newText The new text to display.
   *
   * @details
   * This method is inline and noexcept for maximum performance. It does not
   * trigger a repaint; call repaint() if immediate update is required.
   */
  inline void setText(const String& _newText) noexcept
  {
    this->text = _newText;
  }

  //==============================================================================
  /**
   * @brief Gets the current label text.
   *
   * @return The current text as a String.
   *
   * @details
   * This method is marked [[nodiscard]] to encourage correct usage.
   */
  [[nodiscard]] inline String getText() const noexcept { return this->text; }

  //==============================================================================
  /**
   * @brief Sets the label's font colour.
   *
   * @param _colour The new colour to use for the text.
   *
   * @details
   * Sets the colour of the label's text and triggers a repaint.
   */
  inline void setFontColour(const juce::Colour& _colour) noexcept
  {
    // Make sure the colour is actually different before repainting
    if (fontColour != nullptr && fontColour == &_colour) {
      fontColour = &_colour;
      return;
    }

    // Set the font colour and trigger a repaint.
    fontColour = &_colour;
    repaint();
  }

  //==============================================================================
  /**
   * @brief Sets the label's background colour.
   *
   * @param _colour The new colour to use for the background.
   *
   * @details
   * Sets the colour of the label's background and triggers a repaint.
   */
  inline void setBackgroundColour(const juce::Colour& _colour) noexcept
  {
    // Make sure the colour is actually different before repainting
    if (backgroundColour == &_colour) {
      backgroundColour = &_colour;
      return;
    }

    // Set the background colour and trigger a repaint.
    backgroundColour = &_colour;
    repaint();
  }

  //==============================================================================
  /**
   * @brief Sets the raw horizontal padding (unscaled).
   *
   * @param padding The new raw horizontal padding value.
   */
  inline void setRawHorizontalPadding(float padding) noexcept
  {
    rawHorizontalPadding = padding;
    repaint();
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  String text;
  const juce::Font& font;
  const float& fontSize;
  const juce::Colour* fontColour = nullptr;
  const juce::Colour* backgroundColour = nullptr;
  Justification justification;
  bool multiline;

  //==============================================================================
  // Other members
  float rawHorizontalPadding = 0.0f;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Label)
};
} // namespace widget
} // namespace gui
} // namespace dmt