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
 * AbstractDisplay provides a base class for all display components
 * that require custom painting, shadow/border rendering, and repaint timing.
 * Designed for extensibility and real-time GUI performance.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/utility/Scaleable.h"
#include "gui/widget/Shadow.h"
#include "utility/RepaintTimer.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================
/**
 * @brief Abstract base class for display components with shadow, border, and
 * repaint timer.
 *
 * @details
 * This class provides a foundation for custom display components, handling
 * shadow rendering, border drawing, and periodic repainting. Subclasses must
 * implement extendResized, paintDisplay, and prepareNextFrame for their own
 * drawing and layout logic.
 */
class AbstractDisplay
  : public juce::Component
  , public dmt::utility::RepaintTimer
  , public dmt::Scaleable<AbstractDisplay>
{
  //============================================================================
  // Aliases for convenience
  using Shadow = dmt::gui::widget::Shadow;

  //============================================================================
  // General
  using Display = dmt::Settings::Display;
  const juce::Colour& backgroundColour = Display::backgroundColour;

  // Layout
  const float& rawCornerSize = Display::cornerSize;
  const float& rawPadding = Display::padding;

  // Border
  const bool& drawBorder = Display::drawBorder;
  const juce::Colour& borderColour = Display::borderColour;
  const float& rawBorderStrength = Display::borderStrength;

  // Shadows
  const bool& drawOuterShadow = Display::drawOuterShadow;
  const bool& drawInnerShadow = Display::drawInnerShadow;
  const juce::Colour& outerShadowColour = Display::outerShadowColour;
  const juce::Colour& innerShadowColour = Display::innerShadowColour;
  const float& outerShadowRadius = Display::outerShadowRadius;
  const float& innerShadowRadius = Display::innerShadowRadius;

public:
  //==============================================================================
  /**
   * @brief Constructs an AbstractDisplay.
   *
   * @details
   * Initializes shadow components and starts the repaint timer.
   * Subclasses should implement their own layout and painting logic.
   */
  inline explicit AbstractDisplay(
    /*juce::AudioProcessorValueTreeState& _apvts*/) noexcept
    : outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true)
  {
    this->startRepaintTimer();
    addAndMakeVisible(outerShadow);
    addAndMakeVisible(innerShadow);
  }

  //==============================================================================
  /**
   * @brief Paints the component, including background, border, and display
   * content.
   *
   * @param _g The graphics context.
   *
   * @details
   * Handles all background and border drawing, then delegates to paintDisplay
   * for custom content. Calls prepareNextFrame at the end of each paint.
   * This method is final and cannot be overridden by subclasses.
   */
  inline void paint(juce::Graphics& _g) override final
  {
    // Precalculation
    const auto borderStrength = rawBorderStrength * size;
    const auto cornerSize = rawCornerSize * size;
    const float outerCornerSize = cornerSize;
    const float innerCornerSize = std::clamp(
      outerCornerSize - (borderStrength * 0.5f), 0.0f, outerCornerSize);

    // Draw background if border is disabled
    if (!drawBorder) {
      _g.setColour(backgroundColour);
      _g.fillRoundedRectangle(outerBounds.toFloat(), outerCornerSize);
    }

    // Draw background and border if border is enabled
    if (drawBorder) {
      _g.setColour(borderColour);
      _g.fillRoundedRectangle(outerBounds.toFloat(), outerCornerSize);
      _g.setColour(backgroundColour);
      _g.fillRoundedRectangle(innerBounds.toFloat(), innerCornerSize);
    }
    // Draw display
    paintDisplay(_g, innerBounds);

    // We need to draw the border again because drawing it once didn't cut it
    if (drawBorder) {
      _g.setColour(borderColour);
      const auto borderBounds = outerBounds.reduced(borderStrength / 2.0f);
      _g.drawRoundedRectangle(
        borderBounds.toFloat(), outerCornerSize, borderStrength);
    }

    // Prepare next frame
    prepareNextFrame();
  }

  //==============================================================================
  /**
   * @brief Handles resizing and layout of the component and its shadows.
   *
   * @details
   * Updates bounds for outer/inner shadows and calls extendResized for
   * subclass-specific layout. This method is final and cannot be overridden.
   */
  inline void resized() override final
  {
    TRACER("DisfluxDisplay::resized");

    const auto bounds = getLocalBounds();
    const auto borderStrength = rawBorderStrength * size;
    const auto cornerSize = rawCornerSize * size;
    const auto padding = rawPadding * size;

    outerBounds = bounds.reduced(static_cast<int>(padding));
    innerBounds = outerBounds.reduced(static_cast<int>(borderStrength));
    const float outerCornerSize = cornerSize;
    const float innerCornerSize = std::clamp(
      outerCornerSize - (borderStrength * 0.5f), 0.0f, outerCornerSize);

    juce::Path outerShadowPath;
    outerShadowPath.addRoundedRectangle(outerBounds, outerCornerSize);
    outerShadow.setPath(outerShadowPath);
    outerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    outerShadow.toBack();

    juce::Path innerShadowPath;
    innerShadowPath.addRoundedRectangle(innerBounds, innerCornerSize);
    innerShadow.setPath(innerShadowPath);
    innerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    innerShadow.toBack();

    // Call the childs extendResized method
    extendResized(drawBorder ? innerBounds : outerBounds);
  }

protected:
  //==============================================================================
  /**
   * @brief Extension point for subclasses to handle resizing and layout.
   *
   * @param _displayBounds The bounds of the display area.
   *
   * @details
   * Subclasses must implement this to layout their own subcomponents.
   */
  virtual void extendResized(
    const juce::Rectangle<int>& _displayBounds) noexcept = 0;

  //==============================================================================
  /**
   * @brief Paints the display content.
   *
   * @param _g The graphics context.
   * @param _displayBounds The bounds of the display area.
   *
   * @details
   * Subclasses must implement this to draw their own content.
   * Do not call this method directly.
   */
  virtual void paintDisplay(
    juce::Graphics& _g,
    const juce::Rectangle<int>& _displayBounds) noexcept = 0;

  //==============================================================================
  /**
   * @brief Prepares the next frame for display.
   *
   * @details
   * Subclasses must implement this for any per-frame logic.
   * Do not draw or perform heavy calculations here.
   */
  virtual void prepareNextFrame() noexcept = 0;

private:
  //==============================================================================
  /**
   * @brief Called by the repaint timer to trigger a repaint.
   *
   * @details
   * This override is final and cannot be changed by subclasses.
   */
  inline void repaintTimerCallback() noexcept override final
  {
    this->repaint();
  }

  //==============================================================================
  // Members initialized in the initializer list
  Shadow outerShadow =
    Shadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false);
  Shadow innerShadow =
    Shadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true);

  //==============================================================================
  // Other members
  juce::Rectangle<int> innerBounds;
  juce::Rectangle<int> outerBounds;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractDisplay)
};

} // namespace component
} // namespace gui
} // namespace dmt