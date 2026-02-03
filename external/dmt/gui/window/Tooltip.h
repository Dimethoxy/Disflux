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
 * Tooltip overlay component for displaying contextual help in DMT GUIs.
 * Designed for real-time performance and DPI-aware rendering.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/gui/widget/Shadow.h"
#include "dmt/utility/Fonts.h"
#include "dmt/utility/RepaintTimer.h"
#include "dmt/utility/Scaleable.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace window {

//==============================================================================
/**
 * @brief Tooltip overlay for contextual help in DMT GUIs.
 *
 * @details
 * This class provides a high-performance, DPI-aware tooltip overlay for
 * displaying contextual help text. It uses a custom repaint timer to minimize
 * overhead and supports shadow rendering for visual clarity. The tooltip
 * automatically tracks mouse position and updates its content by traversing
 * the component hierarchy for JUCE TooltipClient instances.
 *
 * Intended for use as a top-level overlay in DMT-based applications.
 */
class Tooltip
  : public juce::Component
  , public dmt::utility::RepaintTimer
  , public dmt::Scaleable<Tooltip>
{
  //==============================================================================
  template<typename T>
  using Rectangle = juce::Rectangle<T>;
  using Settings = dmt::Settings;
  using Fonts = dmt::utility::Fonts;
  using TooltipSettings = dmt::Settings::Tooltip;
  using Shadow = dmt::gui::widget::Shadow;

  //==============================================================================
  // Tooltip

  const Colour& backgroundColour = TooltipSettings::backgroundColour;
  const Colour& borderColour = TooltipSettings::borderColour;
  const Colour& fontColour = TooltipSettings::fontColour;
  const Colour& innerShadowColour = TooltipSettings::innerShadowColour;
  const Colour& outerShadowColour = TooltipSettings::outerShadowColour;
  const float& rawCornerRadius = TooltipSettings::cornerRadius;
  const float& rawBorderWidth = TooltipSettings::borderWidth;
  const float& innerShadowRadius = TooltipSettings::innerShadowRadius;
  const float& outerShadowRadius = TooltipSettings::outerShadowRadius;
  const float& rawFontSize = TooltipSettings::fontSize;
  const float& rawTextHorizontalPadding =
    TooltipSettings::textHorizontalPadding;
  const float& rawTextVerticalPadding = TooltipSettings::textVerticalPadding;
  const bool& drawOuterShadow = TooltipSettings::drawOuterShadow;
  const bool& drawInnerShadow = TooltipSettings::drawInnerShadow;

public:
  //==============================================================================
  /**
   * @brief Constructs a Tooltip overlay component.
   *
   * @details
   * Sets up shadow rendering and starts the custom repaint timer. Mouse clicks
   * are not intercepted to allow interaction with underlying components.
   */
  inline Tooltip() noexcept
    : outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true)
  {
    TRACER("Tooltip::Tooltip");
    setInterceptsMouseClicks(false, false);
    startRepaintTimer();
  }

  //==============================================================================
  /**
   * @brief Destructor.
   */
  inline ~Tooltip() override = default;

  //==============================================================================
  /**
   * @brief Handles resizing of the tooltip overlay.
   *
   * @details
   * If a tooltip image is present, triggers a re-render to ensure DPI and
   * layout correctness after a size change.
   */
  inline void resized() override
  {
    TRACER("Tooltip::resized");
    if (!tooltipImage.isNull())
      renderTooltipImage(currentTooltipText);
  }

  //==============================================================================
  /**
   * @brief Paints the tooltip overlay.
   *
   * @param _graphics The graphics context.
   *
   * @details
   * Draws the tooltip image at the current mouse position, flipping to avoid
   * drawing offscreen if necessary.
   */
  inline void paint(juce::Graphics& _graphics) override
  {
    TRACER("Tooltip::paint");
    if (!tooltipImage.isNull()) {
      const int imageWidth = tooltipImage.getWidth();
      const int imageHeight = tooltipImage.getHeight();
      const int width = getWidth();
      const int height = getHeight();

      int drawPositionX = lastMousePosition.x;
      int drawPositionY = lastMousePosition.y;

      // Flip horizontally if tooltip would go off right edge
      if (drawPositionX + imageWidth / scale > width)
        drawPositionX = std::max(
          0, lastMousePosition.x - static_cast<int>(imageWidth / scale));

      // Flip vertically if tooltip would go off bottom edge
      if (drawPositionY + imageHeight / scale > height)
        drawPositionY = std::max(
          0, lastMousePosition.y - static_cast<int>(imageHeight / scale));

      _graphics.drawImage(tooltipImage,
                          static_cast<float>(drawPositionX),
                          static_cast<float>(drawPositionY),
                          imageWidth / scale,
                          imageHeight / scale,
                          0,
                          0,
                          imageWidth,
                          imageHeight);
    }
  }

  //==============================================================================
  /**
   * @brief Called periodically by the custom repaint timer.
   *
   * @details
   * Traverses the parent component hierarchy to find a JUCE TooltipClient
   * under the mouse, updating the tooltip text and image as needed. Only
   * triggers a repaint if the tooltip text or mouse position has changed.
   */
  inline void repaintTimerCallback() noexcept override
  {
    TRACER("Tooltip::repaintTimerCallback");
    // Get the parent component of the tooltip
    auto* parent = getParentComponent();
    if (!parent)
      return;

    // Get the current mouse position relative to the parent component
    auto mousePosition = parent->getMouseXYRelative();
    auto* component = parent->getComponentAt(mousePosition);

    juce::String foundTooltipText;

    // Traverse up the component hierarchy to find a TooltipClient
    while (component != nullptr && component != parent) {
      if (auto* tooltipClient = dynamic_cast<juce::TooltipClient*>(component)) {
        // If a TooltipClient is found, get its tooltip text
        auto tooltipText = tooltipClient->getTooltip();
        if (!tooltipText.isEmpty()) {
          foundTooltipText = tooltipText;
          break;
        }
      }
      component = component->getParentComponent();
    }

    bool needsRepaint = false;

    // Check if the tooltip text has changed
    if (foundTooltipText != currentTooltipText) {
      currentTooltipText = foundTooltipText;

      // If there is new tooltip text, render the tooltip image
      if (currentTooltipText.isNotEmpty())
        renderTooltipImage(currentTooltipText);
      else
        tooltipImage = juce::Image();
      needsRepaint = true;
    }

    // Check if the mouse position has changed
    if (mousePosition != lastMousePosition) {
      lastMousePosition = mousePosition;
      needsRepaint = true;
    }

    // Repaint the tooltip if necessary
    if (needsRepaint)
      repaint();
  }

protected:
  //==============================================================================
  /**
   * @brief Renders the tooltip image for the given text.
   *
   * @param _text The tooltip text to render.
   *
   * @details
   * Calculates layout, DPI scaling, and draws all visual elements including
   * shadows, border, background, and text. Uses AttributedString for
   * high-quality text rendering.
   */
  inline void renderTooltipImage(const juce::String& _text)
  {
    TRACER("Tooltip::renderTooltipImage");
    // Font size and style
    const auto fontSize = rawFontSize * size;

    // Determine font size based on OS
    const auto font = fonts.medium.withHeight(fontSize);

    const auto justification = juce::Justification::centredLeft;

    // Prepare the AttributedString
    juce::AttributedString attributedString(_text);
    attributedString.setFont(font);
    attributedString.setColour(fontColour);
    attributedString.setJustification(justification);
    attributedString.setWordWrap(juce::AttributedString::WordWrap::none);

    // Calculate the size of the text layout
    const float maxWidth = getWidth();
    juce::TextLayout textLayout;
    textLayout.createLayout(attributedString, maxWidth);
    const auto layoutWidth = textLayout.getWidth() * 1.01f;
    const auto layoutHeight = textLayout.getHeight() * 1.01f;

    // Precalculate parameters
    const auto textHorizontalPadding = rawTextHorizontalPadding * size;
    const auto textVerticalPadding = rawTextVerticalPadding * size;
    const auto innerWidth = layoutWidth + textHorizontalPadding * 2;
    const auto innerHeight = layoutHeight + textVerticalPadding * 2;
    const auto borderWidth = rawBorderWidth * size;
    const auto outerWidth = innerWidth + borderWidth * 2;
    const auto outerHeight = innerHeight + borderWidth * 2;
    const auto shadowRadius =
      std::max(outerShadowRadius, innerShadowRadius) * size;
    const auto tooltipWidth = outerWidth + shadowRadius * 2;
    const auto tooltipHeight = outerHeight + shadowRadius * 2;
    const auto cornerRadius = rawCornerRadius * size;
    const auto innerCornerRadius = cornerRadius - borderWidth;

    // HiDPI scaling stuff
    const auto scaledTooltipWidth = juce::roundToInt(tooltipWidth * scale);
    const auto scaledTooltipHeight = juce::roundToInt(tooltipHeight * scale);

    tooltipImage = juce::Image(
      juce::Image::ARGB, scaledTooltipWidth, scaledTooltipHeight, true);
    juce::Graphics graphics(tooltipImage);
    graphics.addTransform(juce::AffineTransform::scale(scale, scale));

    // Bounds
    const auto tooltipBounds =
      Rectangle<float>(0, 0, tooltipWidth, tooltipHeight);
    const auto outerBounds = tooltipBounds.reduced(shadowRadius);
    const auto innerBounds = outerBounds.reduced(borderWidth);
    const auto textBounds =
      innerBounds.reduced(textHorizontalPadding, textVerticalPadding);

    // Path for the tooltip shape (outer and inner)
    juce::Path outerPath, innerPath;
    outerPath.addRoundedRectangle(outerBounds, cornerRadius);
    innerPath.addRoundedRectangle(innerBounds, innerCornerRadius);

    // Draw outer shadow if enabled
    if (drawOuterShadow)
      outerShadow.directDraw(graphics, outerPath);

    // Draw border
    graphics.setColour(borderColour);
    graphics.fillPath(outerPath);

    // Draw inner shadow if enabled
    if (drawInnerShadow)
      innerShadow.directDraw(graphics, innerPath);

    // Background
    graphics.setColour(backgroundColour);
    graphics.fillPath(innerPath);

    // Text
    graphics.setColour(fontColour);
    graphics.setFont(font);
    graphics.drawText(_text, textBounds, justification, true);
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  Shadow outerShadow;
  Shadow innerShadow;

  //==============================================================================
  // Other members
  juce::String currentTooltipText;
  juce::Image tooltipImage;
  juce::Point<int> lastMousePosition;
  Fonts fonts;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tooltip)
};

} // namespace window
} // namespace gui
} // namespace dmt