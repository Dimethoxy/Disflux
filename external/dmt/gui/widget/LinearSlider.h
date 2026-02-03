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
 * Immplements a juce::Slider in the form of a linear slider that can be either
 * horizontal or vertical. It supports multiple slider types (positive,
 * negative, bipolar, selector) and handles platform-specific scaling and visual
 * customization via settings.
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
 * @brief Linear slider widget with custom rendering and multiple types.
 *
 * @details
 * This class provides a customizable linear slider component optimized for
 * real-time audio applications.
 *
 * It supports both horizontal and vertical
 * orientations, with multiple slider types (positive, negative, bipolar,
 * selector).
 *
 * Platform-specific scaling is applied for consistent appearance
 * across operating systems.
 */
class LinearSlider
  : public juce::Slider
  , public dmt::Scaleable<LinearSlider>
{
  //==============================================================================
  // Alias for convenience
  using Settings = dmt::Settings;
  using StrokeType = juce::PathStrokeType;

  //==============================================================================
  // General
  const float& rawPadding = Settings::Slider::padding;

  // Shaft
  const juce::Colour& shaftColour = Settings::Slider::shaftColour;
  const float& rawShaftLineStrength = Settings::Slider::shaftLineStrength;
  const float& rawShaftSize = Settings::Slider::shaftSize;

  // Rail
  const juce::Colour& lowerRailColour = Settings::Slider::lowerRailColour;
  const juce::Colour& upperRailColour = Settings::Slider::upperRailColour;
  const float& rawRailWidth = Settings::Slider::railWidth;
  const float& railSize = Settings::Slider::railSize;

  // Thumb
  const juce::Colour& thumbInnerColour = Settings::Slider::thumbInnerColour;
  const juce::Colour& thumOuterColour = Settings::Slider::thumbOuterColour;
  const float& rawThumbSize = Settings::Slider::thumbSize;
  const float& rawThumbStrength = Settings::Slider::thumbStrength;

public:
  //==============================================================================
  /**
   * @brief Slider type enumeration.
   *
   * @details
   * Determines the visual and functional behavior of the slider. Bipolar mode
   * centers the slider, while selector mode is for discrete values.
   */
  enum Type
  {
    Positive,
    Negative,
    Bipolar,
    Selector
  };

  //==============================================================================
  /**
   * @brief Slider orientation enumeration.
   *
   * @details
   * Specifies whether the slider is rendered horizontally or vertically.
   */
  enum Orientation
  {
    Horizontal,
    Vertical
  };

  //==============================================================================
  /**
   * @brief Constructs a LinearSlider with the specified type and orientation.
   *
   * @param _type The slider type (Positive, Negative, Bipolar, Selector).
   * @param _orientation The slider orientation (Horizontal or Vertical).
   *
   * @details
   * The constructor is constexpr for compile-time optimization and real-time
   * safety. The slider style and text box are set according to orientation.
   * A random initial value is assigned for demonstration or testing.
   */
  explicit LinearSlider(const Type _type,
                        const Orientation _orientation) noexcept
    : juce::Slider()
    , type(_type)
    , orientation(_orientation)
  {
    TRACER("LinearSlider::LinearSlider");
    switch (_orientation) {
      case Orientation::Horizontal: {
        setSliderStyle(juce::Slider::LinearHorizontal);
        break;
      }
      case Orientation::Vertical: {
        setSliderStyle(juce::Slider::LinearVertical);
        break;
      }
      default: {
        break;
      }
    }
    setTextBoxStyle(juce::Slider::TextBoxBelow, true, 0, 0);
    juce::Random rand;
    setValue(10.0f * rand.nextFloat());
  }

  //==============================================================================
  /**
   * @brief Paints the slider, including rails, thumb, and debug overlays.
   *
   * @param _g The graphics context for rendering.
   *
   * @details
   * This method computes all geometry and visual elements in real time,
   * using platform-specific scaling and settings. Debug overlays are drawn
   * if enabled. The function is marked noexcept for real-time safety.
   */
  inline void paint(juce::Graphics& _g) noexcept override
  {
    TRACER("LinearSlider::paint");

    // Calculate bounds
    auto bounds = getLocalBounds();

    // Draw bounds debug
    _g.setColour(juce::Colours::cyan);
    if (Settings::debugBounds)
      _g.drawRect(bounds, 1);

    // Calculate lower rail
    float thumbSize = rawThumbSize * size;
    const auto railBounds = bounds.reduced(static_cast<int>(thumbSize / 2.0f));
    float primaryPointX;
    float primaryPointY;
    float secondaryPointX;
    float secondaryPointY;
    switch (orientation) {
      case Orientation::Horizontal:
        primaryPointX = static_cast<float>(railBounds.getX());
        primaryPointY = static_cast<float>(railBounds.getCentreY());
        secondaryPointX = static_cast<float>(railBounds.getRight());
        secondaryPointY = static_cast<float>(railBounds.getCentreY());
        break;
      case Orientation::Vertical:
        primaryPointX = static_cast<float>(railBounds.getCentreX());
        primaryPointY = static_cast<float>(railBounds.getBottom());
        secondaryPointX = static_cast<float>(railBounds.getCentreX());
        secondaryPointY = static_cast<float>(railBounds.getY());
        break;
      default:
        jassert(false);
        return;
    }

    // Anchor points
    const juce::Point<float> primaryPoint(primaryPointX, primaryPointY);
    const juce::Point<float> secondaryPoint(secondaryPointX, secondaryPointY);

    // Debug draw anchor points and rail bounds
    if (Settings::debugBounds) {
      _g.setColour(juce::Colours::red);
      _g.drawRect(railBounds, 1);
      _g.setColour(juce::Colours::yellow);
      _g.fillEllipse(primaryPoint.getX() - 8, primaryPoint.getY() - 8, 16, 16);
      _g.fillEllipse(
        secondaryPoint.getX() - 8, secondaryPoint.getY() - 8, 16, 16);
    }

    // Draw lower rail
    const auto railWidth = rawRailWidth * size;
    const auto jointStyle = StrokeType::curved;
    const auto lowerEndCapStyle = StrokeType::rounded;
    const auto lowerStrokeType =
      StrokeType(railWidth, jointStyle, lowerEndCapStyle);
    auto lowerRailPath = juce::Path();
    lowerRailPath.startNewSubPath(primaryPoint);
    lowerRailPath.lineTo(secondaryPoint);
    _g.setColour(lowerRailColour);
    _g.strokePath(lowerRailPath, lowerStrokeType);

    // Calculate upper rail
    const auto upperEndCapStyle =
      (type == Type::Bipolar) ? StrokeType::butt : StrokeType::rounded;
    const auto upperStrokeType =
      StrokeType(railWidth, jointStyle, upperEndCapStyle);
    const auto value = getValue();
    const auto maximum = getMaximum();
    const auto minimum = getMinimum();
    const auto range = maximum - minimum;
    const auto scew = getSkewFactor();
    const auto valueRatio = (std::pow((value - minimum) / range, scew));
    const auto diffrencePoint = secondaryPoint - primaryPoint;
    const auto valueDiffrencePoint = diffrencePoint * valueRatio;
    const auto valuePoint = primaryPoint + valueDiffrencePoint;
    const auto middlePoint = (primaryPoint + secondaryPoint) / 2.0f;
    auto upperRailStartPoint = primaryPoint;
    switch (type) {
      case Type::Positive:
        upperRailStartPoint = primaryPoint;
        break;
      case Type::Negative:
        upperRailStartPoint = secondaryPoint;
        break;
      case Type::Bipolar:
        upperRailStartPoint = middlePoint;
        break;
      case Type::Selector:
        // TODO: Implement selector type
        // As this isn't implemented yet, we just jassert false and return
        jassertfalse;
        break;
    }
    const auto upperRailEndPoint = valuePoint;
    auto upperRailPath = juce::Path();
    upperRailPath.startNewSubPath(upperRailStartPoint);
    upperRailPath.lineTo(upperRailEndPoint);

    // Draw upper rail
    _g.setColour(upperRailColour);
    _g.strokePath(upperRailPath, upperStrokeType);

    // Draw the Thumb
    const auto thumbPoint = valuePoint;
    if (!isMouseButtonDown())
      thumbSize *= 0.85f;
    const float thumbStrength = rawThumbStrength * size;
    const auto thumbBounds = juce::Rectangle<float>()
                               .withSize(thumbSize, thumbSize)
                               .withCentre(thumbPoint);
    _g.setColour(thumOuterColour);
    _g.fillEllipse(thumbBounds);
    _g.setColour(thumbInnerColour);
    _g.fillEllipse(thumbBounds.reduced(thumbStrength));
  }

  //==============================================================================
  /**
   * @brief Callback type for context menu requests.
   */
  std::function<void()> onContextMenuRequested;

  //==============================================================================
  /**
   * @brief Handles mouse down events.
   *
   * @param e The mouse event.
   *
   * @details
   * Detects right-clicks and triggers the context menu callback if set.
   */
  void mouseDown(const juce::MouseEvent& e) override
  {
    auto modifiers = e.mods;
    if (modifiers.isRightButtonDown()) {
      if (onContextMenuRequested)
        onContextMenuRequested();
      return;
    }
    juce::Slider::mouseDown(e);
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  Type type;
  Orientation orientation;

  //==============================================================================
  // Other members

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LinearSlider)
};
} // namespace widget
} // namespace gui
} // namespace dmt