//==============================================================================
/* ██████╗ ██╗███╗   ███╗███████╗████████╗██╗  ██╗ ██████╗ ██╗  ██╗██╗   ██╗
 * ██╔══██╗██║████╗ ████║██╔════╝╚══██╔══╝██║  ██║██╔═══██╗╚██╗██╔╝╚██╗ ██╔╝
 * ██║  ██║██║██╔████╔██║█████╗     ██║   ███████║██║   ██║ ╚███╔╝  ╚████╔╝
 * ██║  ██║██║██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██║   ██║ ██╔██╗   ╚██╔╝
 * ██████╔╝██║██║ ╚═╝ ██║███████╗   ██║   ██║  ██║╚██████╔╝██╔╝ ██╗   ██║
 * ╚═════╝ ╚═╝╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * Part of the Dimethoxy Library, primarily intended for Dimethoxy plugins.
 * External use is permitted but not recommended.
 * No support or compatibility guarantees are provided.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * LinearSliderComponent provides a composite slider UI element with
 * optional SVG title, parameter binding, and context menu support.
 * Designed for real-time audio plugin GUIs.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/component/AbstractSliderComponent.h"
#include "gui/widget/Label.h"
#include "gui/widget/LinearSlider.h"
#include "utility/Fonts.h"
#include "utility/HostContextMenu.h"
#include "utility/Icon.h"
#include "utility/Settings.h"
#include "utility/Unit.h"
#include <JuceHeader.h>

//==============================================================================

// Slider graphics and labels are always included; remove conditional macros

//==============================================================================

namespace dmt {
namespace gui {
namespace component {

//==============================================================================
/**
 * @brief Composite slider component with optional SVG title and parameter
 * binding.
 *
 * @details
 * This component encapsulates a slider, title label (or SVG icon), and info
 * label. It supports parameter attachment, context menu integration, and
 * flexible layout. Designed for use in real-time audio plugin GUIs, with a
 * focus on performance and type safety. The component is non-copyable and
 * leak-detectable.
 */
class LinearSliderComponent
  : public AbstractSliderComponent<LinearSliderComponent,
                                   dmt::gui::widget::LinearSlider>
{
  using LinearSlider = dmt::gui::widget::LinearSlider;
  using Type = LinearSlider::Type;
  using Orientation = LinearSlider::Orientation;
  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

public:
  /**
   * @brief Constructs a LinearSliderComponent.
   *
   * @param _apvts Reference to the AudioProcessorValueTreeState for parameter
   * binding.
   * @param _text The title text for the slider.
   * @param _param The parameter ID to bind.
   * @param _unitType The unit type for value display.
   * @param _type The slider type (default: Positive).
   * @param _orientation The slider orientation (default: Horizontal).
   * @param _svgTitle If true, uses an SVG icon as the title.
   *
   * @details
   * The constructor sets up the slider, labels, parameter attachment, and
   * context menu. SVG title mode disables the text label and uses an icon
   * instead.
   */
  inline explicit LinearSliderComponent(
    juce::AudioProcessorValueTreeState& _apvts,
    const juce::String _text,
    const juce::String _param,
    const Unit::Type _unitType,
    const Type _type = Type::Positive,
    const Orientation _orientation = Orientation::Horizontal,
    const bool _svgTitle = false) noexcept
    : AbstractSliderComponent(_apvts, _text, _param, _unitType)
    , orientation(_orientation)
    , slider(_type, _orientation)
    , sliderAttachment(_apvts, _param, slider)
    , svgTitle(_svgTitle)
    , svgPadding(dmt::icons::getPadding(_param))
  {
    TRACER("LinearSliderComponent::LinearSliderComponent");
    slider.addListener(this);
    updateLabel(static_cast<float>(slider.getValue()));
    addAndMakeVisible(slider);
    addAndMakeVisible(this->infoLabel);

    if (_svgTitle) {
      titleIcon = dmt::icons::getIcon(_param);
      this->titleLabel.setVisible(false);
    }

    this->parameter = _apvts.getParameter(_param);

    slider.onContextMenuRequested = [this]() {
      this->showContextMenuForSlider();
    };
  }

  /**
   * @brief Lays out the child components.
   *
   * @details
   * Arranges the slider, title label or icon, and info label according to
   * orientation. Padding and font sizes are scaled for DPI awareness.
   */
  inline void resized() override
  {
    TRACER("LinearSliderComponent::resized");
    const auto bounds = getLocalBounds();
    const auto padding = rawPadding * this->size;

    slider.setAlwaysOnTop(true);
    switch (orientation) {
      case Orientation::Horizontal: {
        layoutHorizontal(bounds, padding);
        break;
      }
      case Orientation::Vertical: {
        layoutVertical(bounds, padding);
        break;
      }
    }
  }

  /**
   * @brief Lays out the child components for horizontal orientation.
   *
   * @param bounds The bounds of the component.
   * @param padding The padding to apply around the components.
   *
   * @details
   * Arranges the slider in the center with title and info labels above and
   * below, respectively. Padding and font sizes are scaled for DPI awareness.
   */
  void layoutHorizontal(juce::Rectangle<int> bounds, float padding) noexcept
  {
    // Calculate the center point and offset for the slider
    const int rawHorizontalSliderOffset = static_cast<int>(this->size);
    const juce::Point<int> offset(0, rawHorizontalSliderOffset);
    const auto centre = bounds.getCentre() + offset;

    // Main slider bounds
    auto sliderBounds =
      bounds.reduced(static_cast<int>(padding)).withCentre(centre);
    slider.setBounds(
      bounds.reduced(static_cast<int>(padding)).withCentre(centre));

    // Title label bounds
    auto titleLabelBounds = sliderBounds;
    const auto titleLabelHeight =
      static_cast<int>(2 * titleFontSize * this->size);
    const auto titleLabelOffset = static_cast<int>(4 * this->size);
    const auto titleSliderBounds =
      titleLabelBounds.removeFromTop(titleLabelHeight)
        .reduced(titleLabelOffset);
    this->titleLabel.setBounds(titleSliderBounds);

    // Info label bounds
    auto infoLabelBounds = sliderBounds;
    const auto infoLabelHeight =
      static_cast<int>(2 * infoFontSize * this->size);
    const auto infoLabelOffset = static_cast<int>(9 * this->size);
    const auto infoSliderBounds =
      infoLabelBounds.removeFromBottom(infoLabelHeight)
        .reduced(infoLabelOffset);
    this->infoLabel.setBounds(infoSliderBounds);
  }

  /**
   * @brief Lays out the child components for vertical orientation.
   *
   * @param bounds The bounds of the component.
   * @param padding The padding to apply around the components.
   *
   * @details
   * Arranges the slider in the center with title and info labels above and
   * below, respectively. Padding and font sizes are scaled for DPI awareness.
   */
  void layoutVertical(juce::Rectangle<int> bounds, float padding) noexcept
  {
    this->titleLabel.setBounds(
      bounds.withTrimmedTop(static_cast<int>(padding)));
    this->infoLabel.setBounds(
      bounds.withTrimmedBottom(static_cast<int>(padding)));

    auto sliderBounds = bounds;
    sliderBounds.removeFromTop(
      static_cast<int>(titleFontSize * this->size + padding));
    sliderBounds.removeFromBottom(
      static_cast<int>(infoFontSize * this->size + padding));
    slider.setBounds(sliderBounds);
  }

  /**
   * @brief Paints the component, including optional SVG icon.
   *
   * @param _g The graphics context.
   *
   * @details
   * Draws debug bounds if enabled. If SVG title mode is active, draws the icon
   * in the title area with appropriate padding.
   */
  inline void paint(juce::Graphics& _g) override
  {
    TRACER("LinearSliderComponent::paint");
    auto bounds = getLocalBounds();

    // Draw bounds debug
    _g.setColour(juce::Colours::green);
    if (Settings::debugBounds)
      _g.drawRect(bounds, 1);

    constexpr float baseSvgPadding = 2.0f;
    if (svgTitle) {
      juce::Rectangle<float> iconArea =
        bounds.removeFromTop(slider.getY()).toFloat();
      iconArea = iconArea.withY(iconArea.getY() + 6.0f * this->size);
      iconArea = iconArea.reduced((svgPadding + baseSvgPadding) * this->size);
      titleIcon->drawWithin(
        _g, iconArea, juce::RectanglePlacement::centred, 1.0f);
    }
  }

  /**
   * @brief Called when the slider value changes.
   *
   * @param _slider The slider that changed (unused).
   * @details Updates the info label to reflect the new value.
   */
  inline void sliderValueChanged(juce::Slider* /*_slider*/) override
  {
    TRACER("LinearSliderComponent::sliderValueChanged");
    updateLabel(static_cast<float>(slider.getValue()));
  }

  /**
   * @brief Sets the bounds of the component based on two points.
   *
   * @param _primaryPoint The first anchor point.
   * @param _secondaryPoint The second anchor point.
   *
   * @details
   * Used for interactive placement or resizing. Ensures minimum size and
   * orientation-specific layout.
   */
  inline void setBoundsByPoints(juce::Point<int> _primaryPoint,
                                juce::Point<int> _secondaryPoint) noexcept
  {
    TRACER("LinearSliderComponent::setBoundsByPoints");
    const float padding = 2.0f * rawPadding * this->size;
    const float minHeight = 50 * this->size;
    const float minWidth = 40 * this->size;

    const auto centre = (_primaryPoint + _secondaryPoint).toFloat() / 2.0f;
    const int pointDistance = _primaryPoint.getDistanceFrom(_secondaryPoint);

    switch (orientation) {
      case Orientation::Horizontal: {
        setBounds(juce::Rectangle<int>()
                    .withSize(pointDistance, static_cast<int>(minHeight))
                    .expanded(static_cast<int>(padding))
                    .withCentre(centre.toInt()));
        return;
      }
      case Orientation::Vertical: {
        setBounds(juce::Rectangle<int>()
                    .withSize(static_cast<int>(minWidth), pointDistance)
                    .expanded(static_cast<int>(padding))
                    .withCentre(centre.toInt()));
        return;
      }
    }
  }

  /**
   * @brief Returns a reference to the internal slider.
   *
   * @return Reference to the LinearSlider.
   *
   * @details
   * Allows external code to access or customize the slider directly.
   */
  [[nodiscard]] inline juce::Slider& getSlider() noexcept
  {
    TRACER("LinearSliderComponent::getSlider");
    return slider;
  }

private:
  LinearSlider slider;
  SliderAttachment sliderAttachment;
  Orientation orientation;
  const bool svgTitle;
  const float svgPadding;
  std::unique_ptr<juce::Drawable> titleIcon;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LinearSliderComponent)
};
//==============================================================================
} // namespace component
} // namespace gui
} // namespace dmt