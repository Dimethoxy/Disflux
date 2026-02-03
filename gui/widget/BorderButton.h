//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ██    ███████ ██    ██   ███     ████
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
 * This class implements a buttom to bring back the header when it is hidden.
 * It uses a fade-in/out effect for the button's opacity.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/utility/Fonts.h"
#include "utility/RepaintTimer.h"
#include "utility/Scaleable.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================
/**
 * @brief A custom button with fade-in/out effects and border styling.
 *
 * @details This class implements a buttom to bring back the header when it is
 *          hidden.
 *          It uses a fade-in/out effect for the button's opacity.
 */
class BorderButton
  : public juce::Button
  , public dmt::Scaleable<BorderButton>
  , private dmt::utility::RepaintTimer
{
  using Settings = dmt::Settings;
  using Colour = juce::Colour;
  using Graphics = juce::Graphics;
  using Image = juce::Image;
  using Rectangle = juce::Rectangle<int>;
  using Fonts = dmt::utility::Fonts;

  //==============================================================================
  // Header
  const Colour& backgroundColour =
    Settings::Header::borderButtonBackgroundColour;
  const Colour& fontColour = Settings::Header::borderButtonFontColour;
  const float& rawFontSize = Settings::Header::borderButtonFontSize;

  // Constants for opacity and raw fade speed
  static constexpr float MAX_OPACITY = 1.0f;    // Fully visible
  static constexpr float MIN_OPACITY = 0.0f;    // Semi-transparent
  static constexpr float RAW_FADE_SPEED = 0.8f; // Base fade speed (per second)

public:
  using ButtonCallback = std::function<void()>;

  //==============================================================================
  /**
   * @brief Constructs a BorderButton instance.
   *
   * @details Initializes the button with semi-transparency and starts the
   * repaint timer for fade-in/out effects.
   */
  BorderButton() noexcept
    : juce::Button("BorderButton")
    , currentOpacity(MIN_OPACITY) // Start with semi-transparency
    , isHovered(false)
  {
    TRACER("BorderButton::BorderButton");
    addMouseListener(this, true);
    startRepaintTimer(); // Start the timer in the constructor
  }

  //==============================================================================
  /**
   * @brief Destructor for BorderButton.
   */
  ~BorderButton() override = default;

  //==============================================================================
  /**
   * @brief Handles resizing of the button.
   *
   * @details Updates the cached image whenever the button is resized to ensure
   * proper rendering.
   */
  void resized() override
  {
    TRACER("BorderButton::resized");
    updateCachedImage();
  }

  //==============================================================================
  /**
   * @brief Paints the button with the current opacity.
   *
   * @param _g The graphics context.
   * @param _isMouseOverButton Unused parameter.
   * @param _isButtonDown Unused parameter.
   *
   * @details Draws the cached image with the current opacity level for
   * optimized rendering.
   */
  inline void paintButton(Graphics& _g,
                          bool /*_isMouseOverButton*/,
                          bool /*_isButtonDown*/) override
  {
    TRACER("BorderButton::paintButton");
    _g.setOpacity(currentOpacity);
    // Draw the cached image scaled to fit the button area
    _g.drawImage(cachedImage, getLocalBounds().toFloat());
  }

  //==============================================================================
  /**
   * @brief Handles mouse enter events.
   *
   * @param _event The mouse event.
   *
   * @details Instantly sets the button to maximum opacity when hovered.
   */
  inline void mouseEnter(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("BorderButton::mouseEnter");
    isHovered = true;
    currentOpacity = MAX_OPACITY;
    repaint();
  }

  //==============================================================================
  /**
   * @brief Handles mouse exit events.
   *
   * @param _event The mouse event.
   *
   * @details Stops the hover effect when the mouse leaves the button.
   */
  inline void mouseExit(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("BorderButton::mouseExit");
    isHovered = false;
  }

  //==============================================================================
  /**
   * @brief Sets the button's opacity to maximum.
   *
   * @details This method is useful for programmatically forcing the button to
   * appear fully visible.
   */
  inline void setOpacityToMax() noexcept
  {
    TRACER("BorderButton::setOpacityToMax");
    currentOpacity = MAX_OPACITY;
    repaint();
  }

  //==============================================================================
  /**
   * @brief Sets the callback to be invoked when the button is clicked.
   *
   * @param _callback The callback function.
   */
  inline void setButtonCallback(ButtonCallback _callback) noexcept
  {
    TRACER("BorderButton::setButtonCallback");
    buttonCallback = std::move(_callback);
  }

  //==============================================================================
  /**
   * @brief Handles button click events.
   *
   * @details Invokes the user-defined callback function if it is set.
   */
  inline void clicked() override
  {
    TRACER("BorderButton::clicked");
    if (buttonCallback) {
      buttonCallback();
    }
  }

protected:
  //==============================================================================
  /**
   * @brief Updates the cached image for optimized rendering.
   *
   * @details This method is called whenever the button is resized to ensure
   * the cached image matches the new dimensions.
   */
  void updateCachedImage()
  {
    TRACER("BorderButton::updateCachedImage");
    const auto width = getWidth();
    const auto height = getHeight();

    if (width <= 0 || height <= 0) {
      return; // Avoid invalid dimensions
    }

    // HiDPI support: render cached image at higher resolution
    const int hiResWidth = static_cast<int>(width * scale);
    const int hiResHeight = static_cast<int>(height * scale);

    cachedImage = Image(juce::Image::ARGB, hiResWidth, hiResHeight, true);
    Graphics g(cachedImage);

    g.addTransform(juce::AffineTransform::scale(scale, scale));
    g.fillAll(juce::Colours::transparentBlack);
    g.fillAll(backgroundColour);

    const auto fontSize = static_cast<int>(rawFontSize * size);
    const auto font = fonts.medium.withHeight(fontSize);

    g.setFont(font);
    g.setColour(fontColour);
    g.drawText("Click to Show Header",
               juce::Rectangle<int>(0, 0, width, height),
               juce::Justification::centred);
  }

  //==============================================================================
  /**
   * @brief Callback for the repaint timer.
   *
   * @details Adjusts the button's opacity based on hover state and triggers a
   * repaint if necessary.
   */
  void repaintTimerCallback() noexcept override
  {
    TRACER("BorderButton::repaintTimerCallback");
    const float fadeSpeed =
      RAW_FADE_SPEED / static_cast<float>(Settings::framerate);

    if (!isHovered) {
      currentOpacity = std::max(MIN_OPACITY, currentOpacity - fadeSpeed);
      repaint();
    }
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  ButtonCallback buttonCallback; // Callback to be invoked on click
  float currentOpacity;          // Current transparency level
  bool isHovered;                // Whether the button is hovered

  //==============================================================================
  // Other members
  Fonts fonts;
  Image cachedImage; // Cached image for optimized rendering

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BorderButton)
};
} // namespace widget
} // namespace gui
} // namespace dmt