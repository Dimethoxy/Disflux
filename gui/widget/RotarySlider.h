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
 * Implements a juce::Slider in the form of a rotary slider. It supports
 * multiple slider types (positive, negative, bipolar, selector) and handles
 * platform-specific scaling and visual customization via settings.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/utility/Scaleable.h"
#include "utility/Math.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================
/**
 * @brief Rotary slider widget with custom rendering and multiple types.
 *
 * @details
 * This class provides a customizable rotary slider component optimized for
 * real-time audio applications.
 *
 * It supports multiple slider types (positive, negative, bipolar, selector).
 *
 * Platform-specific scaling is applied for consistent appearance.
 */
class RotarySlider
  : public juce::Slider
  , public dmt::Scaleable<RotarySlider>
{
  //==============================================================================
  // Alias for convenience
  using Settings = dmt::Settings;
  using StrokeType = juce::PathStrokeType;

  //==============================================================================
  // General
  const float& rawPadding = Settings::Settings::Slider::padding;

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

  // Selections
  const juce::Colour& selectionOuterColour =
    Settings::Slider::selectionOuterColour;
  const juce::Colour& selectionInnerColour =
    Settings::Slider::selectionInnerColour;
  const juce::Colour& selectionActiveColour =
    Settings::Slider::selectionActiveColour;
  const float& rawSelectionWidth = Settings::Slider::selectionWidth;
  const float& rawSelectionSize = Settings::Slider::selectionSize;
  const float& rawSelectionActivePadding =
    Settings::Slider::selectionActivePadding;

public:
  //==============================================================================
  /**
   * @brief The type of rotary slider.
   */
  enum class Type
  {
    Positive, /**< Only positive values, arc from min to value. */
    Negative, /**< Only negative values, arc from max to value. */
    Bipolar,  /**< Bipolar values, arc from center to value. */
    Selector  /**< Discrete selector, draws selection dots. */
  };

  //==============================================================================
  /**
   * @brief Constructs a RotarySlider of the given type.
   *
   * @param _type The slider type (Positive, Negative, Bipolar, Selector).
   *
   * @details
   * Initializes the slider with rotary style, disables the text box,
   * randomizes the initial value, and sets velocity/skew for real-time feel.
   * The constructor is constexpr for macOS compatibility.
   */
  explicit RotarySlider(Type _type) noexcept
    : juce::Slider()
    , type(_type)
  {
    TRACER("RotarySlider::RotarySlider");
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, true, 0, 0);
    juce::Random rand;
    setValue(static_cast<double>(10.0f * rand.nextFloat()));
    setSkewFactor(2.0);
    setVelocityModeParameters(
      1.0, 1, 0.0, true, juce::ModifierKeys::shiftModifier);
  }

  //==============================================================================
  /**
   * @brief Paints the rotary slider.
   *
   * @param _g The graphics context.
   *
   * @details
   * Handles all drawing for the slider, including debug bounds if enabled.
   * Delegates main rendering to drawSlider().
   */
  inline void paint(juce::Graphics& _g) override
  {
    TRACER("RotarySlider::paint");
    const auto bounds = getLocalBounds().toFloat();
    const auto padding = rawPadding * size;

    // Draw bounds debug
    _g.setColour(juce::Colours::yellow);
    if (Settings::debugBounds)
      _g.drawRect(bounds, 1.0f);

    drawSlider(_g, bounds.reduced(padding));
  }

  //==============================================================================
  /**
   * @brief Gets the type of this rotary slider.
   *
   * @return The slider type.
   */
  [[nodiscard]] inline Type getType() const noexcept
  {
    TRACER("RotarySlider::getType");
    return type;
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

protected:
  //==============================================================================
  /**
   * @brief Draws the rotary slider's visual elements.
   *
   * @param _g The graphics context.
   * @param _bounds The bounds to draw within.
   *
   * @details
   * Renders the shaft, tick, rails, thumb, and selector dots as appropriate
   * for the current slider type and value. All geometry is calculated
   * relative to the provided bounds for resolution independence.
   */
  inline void drawSlider(juce::Graphics& _g,
                         const juce::Rectangle<float>& _bounds) const noexcept
  {
    TRACER("RotarySlider::drawSlider");
    // Draw bounds debug
    _g.setColour(juce::Colours::aqua);
    if (Settings::debugBounds)
      _g.drawRect(_bounds, 1.0f);

    // Draw the shaft
    const auto shaftSize = rawShaftSize * _bounds.getHeight();
    auto shaftBounds = _bounds;
    shaftBounds.setSize(shaftSize, shaftSize);
    shaftBounds.setCentre(_bounds.getCentre());
    const float lineStrength = rawShaftLineStrength * size;
    const auto rawCentre = shaftBounds.getCentre();
    const float centreOffset = shaftBounds.getHeight() / 6.5f;
    const float centreY = rawCentre.getY() + centreOffset;
    const auto centre = juce::Point<float>(rawCentre.getX(), centreY);
    shaftBounds.setCentre(centre);
    _g.setColour(shaftColour);
    _g.drawEllipse(shaftBounds, lineStrength);

    // Draw the tick
    const float minValue = static_cast<float>(getMinimum());
    const float maxValue = static_cast<float>(getMaximum());
    const float value = static_cast<float>(getValue());
    const float skew = getSkewFactor();
    const float valueRatio =
      (std::pow((value - minValue) / (maxValue - minValue), skew));
    constexpr float normalizedStartAngle = 0.0f;
    constexpr float normalizedEndAngle = 260.0f;
    constexpr float angleRange = normalizedEndAngle - normalizedStartAngle;
    constexpr float gapRange = 360.0f - angleRange;
    constexpr float angleOffset = 180.0f + (gapRange / 2.0f);
    const float rawAngle = juce::jmap(
      valueRatio, 0.0f, 1.0f, normalizedStartAngle, normalizedEndAngle);
    const float valueAngleInRadians =
      dmt::math::degreeToRadians(rawAngle + angleOffset);
    const auto tickLine = getTick(shaftBounds, centre, valueAngleInRadians);
    _g.drawLine(tickLine, lineStrength);

    // Rail and selector
    const auto railBounds = _bounds;
    const auto railRadius = railBounds.getWidth() * railSize / 2.0f;

    // Draw the lower rail
    if (type != Type::Selector) {
      const auto railWidth = rawRailWidth * size;
      const auto jointStyle = StrokeType::curved;
      const auto endCapStyle = StrokeType::rounded;
      const auto strokeType = StrokeType(railWidth, jointStyle, endCapStyle);
      const auto startAngleInRadians =
        dmt::math::degreeToRadians(normalizedStartAngle + angleOffset);
      const auto endAngleInRadians =
        dmt::math::degreeToRadians(normalizedEndAngle + angleOffset);
      const auto lowerRail = getLowerRail(
        centre, railRadius, startAngleInRadians, endAngleInRadians);
      _g.setColour(lowerRailColour);
      _g.strokePath(lowerRail, strokeType);

      // Draw the upper rail
      const auto upperRail = getUpperRail(centre,
                                          railRadius,
                                          startAngleInRadians,
                                          endAngleInRadians,
                                          valueAngleInRadians);
      _g.setColour(upperRailColour);
      _g.strokePath(upperRail, strokeType);
    } else {
      const int32_t numSelections =
        static_cast<int32_t>(maxValue) - static_cast<int32_t>(minValue);
      for (size_t i = 0; i <= static_cast<size_t>(numSelections); i++) {
        const float selectionValue = minValue + static_cast<float>(i);
        const float rawSelectionAngle = juce::jmap(selectionValue,
                                                   minValue,
                                                   maxValue,
                                                   normalizedStartAngle,
                                                   normalizedEndAngle);
        const float selectionAngleInRadians =
          dmt::math::degreeToRadians(rawSelectionAngle + angleOffset);
        const auto slectionCentre =
          dmt::math::pointOnCircle(centre, railRadius, selectionAngleInRadians);
        const float selectionSize = rawSelectionSize * size;
        const auto selectionBounds = juce::Rectangle<float>()
                                       .withSize(selectionSize, selectionSize)
                                       .withCentre(slectionCentre);
        _g.setColour(selectionOuterColour);
        _g.fillEllipse(selectionBounds);

        const auto selectionInnerBounds =
          selectionBounds.reduced(rawSelectionWidth * size);

        _g.setColour(selectionInnerColour);
        _g.fillEllipse(selectionInnerBounds);
      }
    }

    // Draw the Thumb
    const auto thumbPoint =
      dmt::math::pointOnCircle(centre, railRadius, valueAngleInRadians);
    float thumbSize = rawThumbSize * size;
    if (!isMouseButtonDown())
      thumbSize *= 0.85f;
    const float thumbStrength = rawThumbStrength * size;
    const auto thumbBounds = juce::Rectangle<float>()
                               .withSize(thumbSize, thumbSize)
                               .withCentre(thumbPoint);
    const auto thumbInnerBounds = thumbBounds.reduced(thumbStrength);
    _g.setColour(thumOuterColour);
    _g.fillEllipse(thumbBounds);
    _g.setColour(thumbInnerColour);
    _g.fillEllipse(thumbInnerBounds);

    if (type == Type::Selector) {
      const float activePadding = rawSelectionActivePadding * size;
      const auto activeBounds = thumbInnerBounds.reduced(activePadding);
      _g.setColour(selectionActiveColour);
      _g.fillEllipse(activeBounds);
    }
  }

  //==============================================================================
  /**
   * @brief Returns the upper rail path for the current slider value.
   *
   * @param _centre The center point of the arc.
   * @param _arcRadius The radius of the arc.
   * @param _startAngleInRadians The start angle in radians.
   * @param _endAngleInRadians The end angle in radians.
   * @param _valueAngleInRadians The value angle in radians.
   * @return The JUCE path representing the upper rail.
   *
   * @details
   * The arc is drawn differently depending on the slider type.
   * Selector type is not supported and will assert.
   */
  [[nodiscard]] inline const juce::Path getUpperRail(
    const juce::Point<float>& _centre,
    const float _arcRadius,
    const float _startAngleInRadians,
    const float _endAngleInRadians,
    const float _valueAngleInRadians) const noexcept
  {
    TRACER("RotarySlider::getUpperRail");
    juce::Path arc;
    switch (this->type) {
      case Type::Selector:
        jassert("How did we get here?");
        break;
      case Type::Positive:
        arc.addCentredArc(_centre.getX(),
                          _centre.getY(),
                          _arcRadius,
                          _arcRadius,
                          0.0f,
                          _startAngleInRadians,
                          _valueAngleInRadians,
                          true);
        break;
      case Type::Negative:
        arc.addCentredArc(_centre.getX(),
                          _centre.getY(),
                          _arcRadius,
                          _arcRadius,
                          0.0f,
                          _endAngleInRadians,
                          _valueAngleInRadians,
                          true);
        break;
      case Type::Bipolar: {
        const auto centreValue =
          (_endAngleInRadians - _startAngleInRadians) / 2.0f;
        arc.addCentredArc(_centre.getX(),
                          _centre.getY(),
                          _arcRadius,
                          _arcRadius,
                          0.0f,
                          _startAngleInRadians + centreValue,
                          _valueAngleInRadians,
                          true);
      } break;
    }
    return arc;
  }

  //==============================================================================
  /**
   * @brief Returns the lower rail path for the slider.
   *
   * @param _centre The center point of the arc.
   * @param _arcRadius The radius of the arc.
   * @param _startAngleInRadians The start angle in radians.
   * @param _endAngleInRadians The end angle in radians.
   * @return The JUCE path representing the lower rail.
   *
   * @details
   * The lower rail always draws the full arc from start to end.
   */
  [[nodiscard]] inline const juce::Path getLowerRail(
    const juce::Point<float>& _centre,
    float _arcRadius,
    float _startAngleInRadians,
    float _endAngleInRadians) const noexcept
  {
    TRACER("RotarySlider::getLowerRail");
    juce::Path arc;
    arc.addCentredArc(_centre.getX(),
                      _centre.getY(),
                      _arcRadius,
                      _arcRadius,
                      0.0f,
                      _startAngleInRadians,
                      _endAngleInRadians,
                      true);
    return arc;
  }

  //==============================================================================
  /**
   * @brief Returns the tick line for the current value.
   *
   * @param _bounds The bounds of the shaft.
   * @param _centre The center point of the shaft.
   * @param _angleInRadians The angle for the tick in radians.
   * @return The JUCE line representing the tick.
   *
   * @details
   * The tick is drawn from the outer edge of the shaft to a point near the
   * center.
   */
  [[nodiscard]] inline const juce::Line<float> getTick(
    const juce::Rectangle<float>& _bounds,
    const juce::Point<float>& _centre,
    const float& _angleInRadians) const noexcept
  {
    TRACER("RotarySlider::getTick");
    const float outerRadius = _bounds.getWidth() / 2.0f;
    const auto outerPoint =
      dmt::math::pointOnCircle(_centre, outerRadius, _angleInRadians);
    const float innerRadius = outerRadius / 5.0f;
    const auto innerPoint =
      dmt::math::pointOnCircle(_centre, innerRadius, _angleInRadians);
    return juce::Line<float>(outerPoint, innerPoint);
  }

  //==============================================================================
private:
  //==============================================================================
  // Members initialized in the initializer list
  Type type;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotarySlider)
};

} // namespace widget
} // namespace gui
} // namespace dmt
