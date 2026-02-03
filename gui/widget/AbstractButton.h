//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ██   ██ ██    ██   ███     ████
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
 * AbstractButton is a customizable button class that supports shadows, icons,
 * and tooltips. It is optimized for real-time performance and designed for
 * use in GUI applications.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/widget/Shadow.h"
#include "utility/Icon.h"
#include "utility/Scaleable.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================
/**
 * @class AbstractButton
 * @brief A customizable button class with support for shadows, icons, and
 * tooltips.
 *
 * AbstractButton extends the JUCE Button class and provides additional
 * functionality for rendering shadows, icons, and background states.
 *
 * @details This class is designed for GUI applications requiring visually
 * appealing and interactive buttons. It supports hover and click states,
 * customizable colors, and tooltips. The rationale for this design is to
 * centralize button appearance logic and maximize real-time GUI performance.
 */
class AbstractButton
  : public juce::Button
  , public dmt::Scaleable<AbstractButton>
{
  using String = juce::String;
  using Settings = dmt::Settings;
  using Colour = juce::Colour;
  using Image = juce::Image;
  using ImageComponent = juce::ImageComponent;
  using ButtonSettings = dmt::Settings::Button;

  //==============================================================================
  // Button
  const Colour& backgroundColour = ButtonSettings::backgroundColour;
  const Colour& outerShadowColour = ButtonSettings::outerShadowColour;
  const Colour& innerShadowColour = ButtonSettings::innerShadowColour;
  const Colour& fontColour = ButtonSettings::fontColour;
  const Colour& hoverColour = ButtonSettings::hoverColour;
  const Colour& clickColour = ButtonSettings::clickColour;
  const float& rawCornerRadius = ButtonSettings::cornerRadius;
  const float& rawButtonPadding = ButtonSettings::padding;
  const float& outerShadowRadius = ButtonSettings::outerShadowRadius;
  const float& innerShadowRadius = ButtonSettings::innerShadowRadius;
  const bool& drawOuterShadow = ButtonSettings::drawOuterShadow;
  const bool& drawInnerShadow = ButtonSettings::drawInnerShadow;

public:
  //==============================================================================
  /**
   * @brief Constructs an AbstractButton instance.
   *
   * @param _name The name of the button.
   * @param _iconName The name of the icon to display on the button.
   * @param _tooltip The tooltip text for the button.
   * @param _shouldDrawBorder Whether to draw a border around the button.
   * @param _shouldDrawBackground Whether to draw a background for the button.
   * @param _shouldDrawShadow Whether to draw shadows for the button.
   * @param _alternativeIconHover Whether to use hoverColour or black for the
   *                              icons hover state.
   *
   * @details The constructor initializes all visual and interactive states,
   * including icon, shadow, and background components. All members are
   * initialized in the order they appear in the initializer list for
   * performance and clarity.
   */
  explicit AbstractButton(String _name,
                          String _iconName,
                          String _tooltip = "",
                          bool _shouldDrawBorder = true,
                          bool _shouldDrawBackground = true,
                          bool _shouldDrawShadow = true,
                          bool _alternativeIconHover = false) noexcept
    : juce::Button(_name)
    , tooltip(_tooltip)
    , shouldDrawBorder(_shouldDrawBorder)
    , shouldDrawBackground(_shouldDrawBackground)
    , shouldDrawShadows(_shouldDrawShadow)
    , alternativeIconHover(_alternativeIconHover)
    , rawSpecificSvgPadding(dmt::icons::getPadding(_iconName))
    , outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true)
  {
    TRACER("AbstractButton::AbstractButton");
    icon = dmt::icons::getIcon(_iconName);

    if (shouldDrawShadows) {
      addAndMakeVisible(outerShadow);
      addAndMakeVisible(innerShadow);
    }

    if (shouldDrawBackground) {
      addAndMakeVisible(backgroundImageComponent);
      addAndMakeVisible(hoverBackgroundImageComponent);
      addAndMakeVisible(clickedBackgroundImageComponent);
      hoverBackgroundImageComponent.setVisible(false);
      clickedBackgroundImageComponent.setVisible(false);
    }

    addAndMakeVisible(iconImageComponent);
    addAndMakeVisible(hoverIconImageComponent);
    hoverIconImageComponent.setVisible(false);

    addMouseListener(this, true);
  }

  //==============================================================================
  /**
   * @brief Destructor for AbstractButton.
   *
   * @details Virtual and defaulted for safe polymorphic destruction.
   */
  inline ~AbstractButton() override = default;

  //==============================================================================
  /**
   * @brief Resizes the button and its components.
   *
   * @details This method recalculates the bounds for shadows, background, and
   * icons based on the button's size and padding. It is called by the JUCE
   * framework when the component is resized, ensuring all visuals remain
   * pixel-perfect and performant.
   */
  inline void resized() override
  {
    TRACER("AbstractButton::resized");
    auto bounds = getLocalBounds();
    const auto buttonPadding = rawButtonPadding * size;
    auto innerBounds = bounds.reduced(buttonPadding);
    const auto cornerRadius = rawCornerRadius * size;

    setShadowBounds(innerBounds, cornerRadius);
    setBackgroundBounds(innerBounds);
    setIconBounds(innerBounds);
    drawBackground();
    drawIcon();
  }

  //==============================================================================
  /**
   * @brief Retrieves the tooltip text for the button.
   * @return The tooltip text as a String.
   *
   * @details Used by JUCE's tooltip system to display contextual help.
   */
  [[nodiscard]] inline String getTooltip() override
  {
    TRACER("AbstractButton::getTooltip");
    return tooltip;
  }

  //==============================================================================
  /**
   * @brief Sets the button to its passive (default) state.
   *
   * @details This method is called to visually reset the button to its
   * non-hover, non-clicked state. Used for consistent state management.
   */
  inline void setPassiveState()
  {
    TRACER("AbstractButton::setPassiveState");
    if (shouldDrawBackground) {
      backgroundImageComponent.setVisible(true);
      hoverBackgroundImageComponent.setVisible(false);
      clickedBackgroundImageComponent.setVisible(false);
    }
    iconImageComponent.setVisible(true);
    hoverIconImageComponent.setVisible(false);
  }

  //==============================================================================
  /**
   * @brief Sets the button to its hover state.
   *
   * @details Called when the mouse hovers over the button, updating visuals
   * for immediate user feedback.
   */
  inline void setHoverState()
  {
    TRACER("AbstractButton::setHoverState");
    if (shouldDrawBackground) {
      backgroundImageComponent.setVisible(false);
      hoverBackgroundImageComponent.setVisible(true);
      clickedBackgroundImageComponent.setVisible(false);
    }
    iconImageComponent.setVisible(false);
    hoverIconImageComponent.setVisible(true);
  }

  //==============================================================================
  /**
   * @brief Sets the button to its clicked state.
   *
   * @details Called when the button is pressed, updating visuals for
   * immediate user feedback.
   */
  inline void setClickedState()
  {
    TRACER("AbstractButton::setClickedState");
    if (shouldDrawBackground) {
      backgroundImageComponent.setVisible(false);
      hoverBackgroundImageComponent.setVisible(false);
      clickedBackgroundImageComponent.setVisible(true);
    }
    iconImageComponent.setVisible(false);
    hoverIconImageComponent.setVisible(true);
  }

  //==============================================================================
  /**
   * @brief Paints the button. This method is intentionally left empty.
   *
   * @param g The graphics context.
   * @param isMouseOverButton Whether the mouse is over the button.
   * @param isButtonDown Whether the button is pressed.
   *
   * @details All painting is handled by subcomponents for maximum flexibility
   * and performance. This override is required by JUCE.
   */
  inline void paintButton(juce::Graphics& /*_g*/,
                          bool /*_isMouseOverButton*/,
                          bool /*_isButtonDown*/) override
  {
    TRACER("AbstractButton::paintButton");
  }

private:
  //==============================================================================
  /**
   * @brief Sets the bounds for the shadows.
   *
   * @param _innerBounds The inner bounds of the button.
   * @param _cornerRadius The corner radius for the shadows.
   *
   * @details This method ensures that both inner and outer shadows are
   * correctly sized and layered for visual consistency. Uses explicit
   * casting for type safety.
   */
  inline void setShadowBounds(const juce::Rectangle<int>& _innerBounds,
                              float _cornerRadius)
  {
    TRACER("AbstractButton::setShadowBounds");
    if (!shouldDrawShadows)
      return;

    // Set the bounds for the outer shadow
    juce::Path outerShadowPath;
    outerShadowPath.addRoundedRectangle(_innerBounds, _cornerRadius);
    outerShadow.setPath(outerShadowPath);
    outerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);

    // Set the bounds for the inner shadow
    juce::Path innerShadowPath;
    innerShadowPath.addRoundedRectangle(_innerBounds, _cornerRadius);
    innerShadow.setPath(innerShadowPath);
    innerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);

    // Reorder the components
    innerShadow.toBack();
    outerShadow.toBack();
    clickedBackgroundImageComponent.toBack();
    hoverBackgroundImageComponent.toBack();
    backgroundImageComponent.toBack();
  }

  //==============================================================================
  /**
   * @brief Sets the bounds for the background components.
   *
   * @param _innerBounds The inner bounds of the button.
   *
   * @details Ensures that all background images are sized to match the
   * button's inner area for pixel-perfect rendering.
   */
  inline void setBackgroundBounds(const juce::Rectangle<int>& _innerBounds)
  {
    TRACER("AbstractButton::setBackgroundBounds");
    if (!shouldDrawBackground)
      return;

    if (_innerBounds.getWidth() <= 0 || _innerBounds.getHeight() <= 0)
      return;

    // HiDPI support: render background images at higher resolution
    const int hiResWidth = static_cast<int>(_innerBounds.getWidth() * scale);
    const int hiResHeight = static_cast<int>(_innerBounds.getHeight() * scale);

    backgroundImage = Image(Image::ARGB, hiResWidth, hiResHeight, true);
    backgroundImageComponent.setBounds(_innerBounds);
    hoverBackgroundImage = Image(Image::ARGB, hiResWidth, hiResHeight, true);
    hoverBackgroundImageComponent.setBounds(_innerBounds);
    clickedBackgroundImage = Image(Image::ARGB, hiResWidth, hiResHeight, true);
    clickedBackgroundImageComponent.setBounds(_innerBounds);
  }

  //==============================================================================
  /**
   * @brief Sets the bounds for the icon components.
   *
   * @param _innerBounds The inner bounds of the button.
   *
   * @details Calculates icon area using both global and icon-specific
   * padding, ensuring icons are always centered and visually balanced.
   */
  inline void setIconBounds(const juce::Rectangle<int>& _innerBounds)
  {
    TRACER("AbstractButton::setIconBounds");
    const auto specificSvgPadding = rawSpecificSvgPadding * size;
    const auto globalSvgPadding = 2.5f * size;
    const auto svgPadding = specificSvgPadding + globalSvgPadding;

    const auto iconArea = _innerBounds.reduced(svgPadding);

    if (iconArea.getWidth() <= 0 || iconArea.getHeight() <= 0)
      return;

    // Render the icon images at higher resolution
    const int hiResWidth = static_cast<int>(iconArea.getWidth() * scale);
    const int hiResHeight = static_cast<int>(iconArea.getHeight() * scale);

    iconImage = juce::Image(juce::Image::ARGB, hiResWidth, hiResHeight, true);
    iconImageComponent.setBounds(iconArea);

    hoverIconImage =
      juce::Image(juce::Image::ARGB, hiResWidth, hiResHeight, true);
    hoverIconImageComponent.setBounds(iconArea);
  }

  //==============================================================================
  /**
   * @brief Draws the background components.
   *
   * @details Fills each background image with the appropriate color and
   * rounded rectangle, using explicit casting for type safety.
   */
  inline void drawBackground()
  {
    TRACER("AbstractButton::drawBackground");
    if (!shouldDrawBackground)
      return;

    const auto cornerRadius = rawCornerRadius * size;
    // Use the hi-res image bounds, but draw at logical size
    const auto hiResBackgroundArea = backgroundImage.getBounds().toFloat();
    const auto logicalWidth = hiResBackgroundArea.getWidth() / scale;
    const auto logicalHeight = hiResBackgroundArea.getHeight() / scale;
    const juce::Rectangle<float> logicalArea(0, 0, logicalWidth, logicalHeight);

    juce::Graphics backgroundGraphics(backgroundImage);
    backgroundGraphics.addTransform(juce::AffineTransform::scale(scale, scale));
    backgroundGraphics.fillAll(juce::Colours::transparentBlack);
    backgroundGraphics.setColour(backgroundColour);
    backgroundGraphics.fillRoundedRectangle(logicalArea, cornerRadius);
    backgroundImageComponent.setImage(backgroundImage,
                                      juce::RectanglePlacement::stretchToFit);

    juce::Graphics hoverGraphics(hoverBackgroundImage);
    hoverGraphics.addTransform(juce::AffineTransform::scale(scale, scale));
    hoverGraphics.fillAll(juce::Colours::transparentBlack);
    hoverGraphics.setColour(hoverColour);
    hoverGraphics.fillRoundedRectangle(logicalArea, cornerRadius);
    hoverBackgroundImageComponent.setImage(
      hoverBackgroundImage, juce::RectanglePlacement::stretchToFit);

    juce::Graphics clickGraphics(clickedBackgroundImage);
    clickGraphics.addTransform(juce::AffineTransform::scale(scale, scale));
    clickGraphics.fillAll(juce::Colours::transparentBlack);
    clickGraphics.setColour(clickColour);
    clickGraphics.fillRoundedRectangle(logicalArea, cornerRadius);
    clickedBackgroundImageComponent.setImage(
      clickedBackgroundImage, juce::RectanglePlacement::stretchToFit);
  }

  //==============================================================================
  /**
   * @brief Draws the icon components.
   *
   * @details Handles icon color replacement and placement for both normal
   * and hover states, using explicit color replacement for clarity.
   */
  inline void drawIcon()
  {
    TRACER("AbstractButton::drawIcon");
    if (icon != nullptr) {
      // Draw normal icon at high res, then scale down for display
      juce::Graphics iconGraphics(iconImage);
      iconGraphics.addTransform(juce::AffineTransform::scale(scale, scale));
      iconGraphics.fillAll(juce::Colours::transparentBlack);
      const auto iconArea = iconImage.getBounds().toFloat() / scale;
      const auto clonedIcon = icon->createCopy();
      clonedIcon->replaceColour(juce::Colours::black, juce::Colours::white);
      clonedIcon->drawWithin(
        iconGraphics, iconArea, juce::RectanglePlacement::centred, 1.0f);
      iconImageComponent.setImage(iconImage,
                                  juce::RectanglePlacement::stretchToFit);

      // Draw hover icon at high res, then scale down for display
      juce::Graphics hoverIconGraphics(hoverIconImage);
      hoverIconGraphics.addTransform(
        juce::AffineTransform::scale(scale, scale));
      hoverIconGraphics.fillAll(juce::Colours::transparentBlack);
      const auto clonedHoverIcon = icon->createCopy();
      clonedHoverIcon->replaceColour(
        juce::Colours::black,
        (alternativeIconHover) ? hoverColour : juce::Colours::black);
      clonedHoverIcon->drawWithin(
        hoverIconGraphics, iconArea, juce::RectanglePlacement::centred, 1.0f);
      hoverIconImageComponent.setImage(hoverIconImage,
                                       juce::RectanglePlacement::stretchToFit);
    }
  }

  //==============================================================================
  // Members initialized in the initializer list
  String tooltip;
  bool shouldDrawBorder;
  bool shouldDrawBackground;
  bool shouldDrawShadows;
  bool alternativeIconHover;
  const float rawSpecificSvgPadding;
  Shadow outerShadow;
  Shadow innerShadow;

  //==============================================================================
  // Other members
  std::unique_ptr<juce::Drawable> icon;
  Image backgroundImage;
  ImageComponent backgroundImageComponent;
  Image hoverBackgroundImage;
  ImageComponent hoverBackgroundImageComponent;
  Image clickedBackgroundImage;
  ImageComponent clickedBackgroundImageComponent;
  Image iconImage;
  ImageComponent iconImageComponent;
  Image hoverIconImage;
  ImageComponent hoverIconImageComponent;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractButton)
};

} // namespace widget
} // namespace gui
} // namespace dmt