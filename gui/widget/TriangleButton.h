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
 * Triangle shaped button used for carousel to switch between panels.
 * Not really designed for general use.
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
 * @brief
 *   A triangle-shaped button with optional border and shadow effects.
 *
 * @details
 * Triangle shaped button used for carousel to switch between panels.
 * Not really designed for general use.
 */
class TriangleButton
  : public juce::Button
  , public dmt::Scaleable<TriangleButton>
{
  //==============================================================================
  using Shadow = dmt::gui::widget::Shadow;
  using Settings = dmt::Settings;

  //==============================================================================
  const float& margin = Settings::Window::margin; // TODO: Remove this
  // General
  const juce::Colour& standbyColour = Settings::TriangleButton::standbyColour;
  const juce::Colour& hoverColour = Settings::TriangleButton::hoverColour;
  const float& buttonMargin = Settings::TriangleButton::margin;
  const float& toggleReduction = Settings::TriangleButton::toggleReduction;
  // Border
  const bool& drawBorder = Settings::TriangleButton::drawBorder;
  const juce::Colour& borderColour = Settings::TriangleButton::borderColour;
  const float& rawBorderStrength = Settings::TriangleButton::borderStrength;
  // Shadows
  const bool& drawOuterShadow = Settings::TriangleButton::drawOuterShadow;
  const bool& drawInnerShadow = Settings::TriangleButton::drawInnerShadow;
  const juce::Colour& outerShadowColour =
    Settings::TriangleButton::outerShadowColour;
  const juce::Colour& innerShadowColour =
    Settings::TriangleButton::innerShadowColour;
  const float& outerShadowRadius = Settings::TriangleButton::outerShadowRadius;
  const float& innerShadowRadius = Settings::TriangleButton::innerShadowRadius;

public:
  //==============================================================================
  /**
   * @brief
   *   The direction of the triangle.
   */
  enum Direction
  {
    Left,
    Right,
    Top,
    Down
  };

  //==============================================================================
  /**
   * @brief
   *   Constructs a TriangleButton in the given direction.
   *
   * @param _d The direction for the triangle.
   *
   * @details
   *   The constructor is constexpr for macOS compatibility. Shadow and border
   *   settings are taken from Settings. Shadows are added as child components.
   */
  inline TriangleButton(Direction _d) noexcept
    : juce::Button("TriangleButton")
    , direction(_d)
    , outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true)
  {
    TRACER("TriangleButton::TriangleButton");
    addAndMakeVisible(outerShadow);
    addAndMakeVisible(innerShadow);
  }

protected:
  //==============================================================================
  /**
   * @brief
   *   Returns a path representing the triangle in the current direction.
   *
   * @param _bounds The bounds to use for the triangle.
   * @return The triangle path.
   *
   * @details
   *   The triangle is constructed based on the direction and the provided
   * bounds.
   */
  [[nodiscard]] inline juce::Path getPath(juce::Rectangle<int> _bounds)
  {
    TRACER("TriangleButton::getPath");
    juce::Path path;
    const float left = static_cast<float>(_bounds.getX());
    const float right = static_cast<float>(_bounds.getRight());
    const float top = static_cast<float>(_bounds.getY());
    const float bottom = static_cast<float>(_bounds.getBottom());
    const float centreX = static_cast<float>(_bounds.getCentreX());
    const float centreY = static_cast<float>(_bounds.getCentreY());
    switch (direction) {
      case Direction::Left: {
        juce::Point<float> p1(left, centreY);
        juce::Point<float> p2(right, top);
        juce::Point<float> p3(right, bottom);
        path.addTriangle(p1, p2, p3);
        break;
      }
      case Direction::Right: {
        juce::Point<float> p1(left, top);
        juce::Point<float> p2(left, bottom);
        juce::Point<float> p3(right, centreY);
        path.addTriangle(p1, p2, p3);
        break;
      }
      case Direction::Top: {
        juce::Point<float> p1(left, top);
        juce::Point<float> p2(right, top);
        juce::Point<float> p3(centreX, bottom);
        path.addTriangle(p1, p2, p3);
        break;
      }
      case Direction::Down: {
        juce::Point<float> p1(left, top);
        juce::Point<float> p2(right, top);
        juce::Point<float> p3(centreX, bottom);
        path.addTriangle(p1, p2, p3);
        break;
      }
    }
    return path;
  }

  //==============================================================================
  /**
   * @brief
   *   Returns a path for the inner triangle, reduced for border rendering.
   *
   * @param _origin The bounds to use for the inner triangle.
   * @return The inner triangle path, or an empty path if border is disabled.
   *
   * @details
   *   The inner triangle is reduced in size based on the border strength and
   *   aspect ratio, and is centered within the original bounds.
   */
  [[nodiscard]] inline juce::Path getTnnerTrianglePath(
    juce::Rectangle<int> _origin)
  {
    TRACER("TriangleButton::getTnnerTrianglePath");
    if (drawBorder) {
      auto bounds = _origin;
      float width = static_cast<float>(bounds.getWidth());
      float height = static_cast<float>(bounds.getHeight());
      float ratio = width / height;
      float borderStrength = rawBorderStrength * size;
      int reducedWidth = static_cast<int>(bounds.getWidth() - borderStrength);
      int reducedHeight =
        static_cast<int>(bounds.getHeight() - (borderStrength / ratio));
      bounds.setSize(reducedWidth, reducedHeight);
      bounds.setCentre(_origin.getCentre());
      return getPath(bounds);
    } else
      return juce::Path();
  }

  //==============================================================================
  /**
   * @brief
   *   Called when the button's state changes. Triggers a repaint.
   */
  inline void buttonStateChanged() override
  {
    TRACER("TriangleButton::buttonStateChanged");
    repaint();
  }

  //==============================================================================
  /**
   * @brief
   *   Paints the triangle button, including border and fill.
   *
   * @param _g The graphics context.
   * @param _shouldDrawButtonAsHighlighted Indicates if the button should be
   * drawn as highlighted.
   * @param _shouldDrawButtonAsDown Indicates if the button should be drawn as
   * pressed.
   *
   * @details
   *   The triangle is drawn in two sizes depending on the mouse state. Border
   * and fill colors are set according to hover/click state and settings.
   */
  inline void paintButton(juce::Graphics& _g,
                          bool /*_shouldDrawButtonAsHighlighted*/,
                          bool /*_shouldDrawButtonAsDown*/) override
  {
    TRACER("TriangleButton::paintButton");
    const auto bounds = this->getLocalBounds();
    const int bigBoundsPadding = static_cast<int>(buttonMargin * size);
    const auto bigBounds = bounds.reduced(bigBoundsPadding);
    juce::Path outerTrianglePath;
    juce::Path innerTrianglePath;
    juce::Path& trianglePath = outerTrianglePath;

    // Calculate clicked path
    if (isMouseButtonDown()) {
      outerTrianglePath = getPath(bigBounds);
      innerTrianglePath = getTnnerTrianglePath(bigBounds);
    } else {
      auto smallBounds = bigBounds;
      const int smallWeight =
        static_cast<int>(smallBounds.getWidth() * toggleReduction);
      smallBounds.setWidth(smallWeight);
      const int smallHeight =
        static_cast<int>(smallBounds.getHeight() * toggleReduction);
      smallBounds.setHeight(smallHeight);
      smallBounds.setCentre(bigBounds.getCentre());
      outerTrianglePath = getPath(smallBounds);
      innerTrianglePath = getTnnerTrianglePath(smallBounds);
    }

    // Draw Border
    if (drawBorder) {
      _g.setColour(borderColour);
      _g.fillPath(outerTrianglePath);
      trianglePath = innerTrianglePath;
    }

    // Set triangle fill color
    if (isMouseOver() && !isMouseButtonDown())
      _g.setColour(hoverColour);
    else
      _g.setColour(standbyColour);

    // Fill triangle
    _g.fillPath(trianglePath);
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  Direction direction;
  Shadow outerShadow;
  Shadow innerShadow;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriangleButton)
};
//==============================================================================
} // namespace widget
} // namespace gui
} // namespace dmt