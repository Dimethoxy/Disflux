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
 * RotarySliderComponent provides a composite widget for rotary slider controls,
 * including parameter binding, context menu, and value display. Designed for
 * real-time audio plugin GUIs with strict performance and type safety.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/component/AbstractSliderComponent.h"
#include "gui/widget/Label.h"
#include "gui/widget/RotarySlider.h"
#include "utility/Fonts.h"
#include "utility/HostContextMenu.h"
#include "utility/Icon.h"
#include "utility/Settings.h"
#include "utility/Unit.h"
#include <JuceHeader.h>

//==============================================================================

// Define to 1 to exclude the main slider graphics
#define DMT_EXCLUDE_SLIDER_GRAPHICS 0

// Define to 1 to exclude title/info labels
#define DMT_EXCLUDE_SLIDER_LABELS 0

//==============================================================================

namespace dmt {
namespace gui {
namespace component {

//==============================================================================
/**
 * @brief Composite rotary slider component with parameter binding and context
 * menu.
 *
 * @details
 * This class encapsulates a rotary slider, a title label, and an info label
 * displaying the current value with units. It binds to a parameter in a
 * JUCE AudioProcessorValueTreeState for real-time automation and supports
 * host context menus for parameter management.
 *
 * Intended for use in audio plugin GUIs where real-time safety and
 * responsiveness are critical. All layout and painting are optimized for
 * performance and clarity.
 */
class RotarySliderComponent
  : public AbstractSliderComponent<RotarySliderComponent,
                                   dmt::gui::widget::RotarySlider>
{
  using RotarySlider = dmt::gui::widget::RotarySlider;
  using Type = RotarySlider::Type;
  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

public:
  /**
   * @brief Constructs a RotarySliderComponent with parameter binding.
   *
   * @param _apvts Reference to the AudioProcessorValueTreeState for parameter
   * binding.
   * @param _text The label text for the slider.
   * @param _param The parameter ID to bind the slider to.
   * @param _unitType The unit type for value display.
   * @param _type The rotary slider type (default: Positive).
   *
   * @details
   * The constructor sets up the slider, attaches it to the parameter,
   * initializes labels, and configures the context menu callback.
   * All layout and font settings are derived from the Settings singleton.
   */
  inline explicit RotarySliderComponent(
    juce::AudioProcessorValueTreeState& _apvts,
    const juce::String _text,
    const juce::String _param,
    const Unit::Type _unitType,
    const Type _type = Type::Positive) noexcept
    : AbstractSliderComponent(_apvts, _text, _param, _unitType)
    , slider(_type)
    , sliderAttachment(_apvts, _param, slider)
  {
    TRACER("RotarySliderComponent::RotarySliderComponent");
    slider.addListener(this);
    updateLabel(static_cast<float>(slider.getValue()));
#if DMT_EXCLUDE_SLIDER_GRAPHICS == 0
    addAndMakeVisible(slider);
#endif
    // Set up context menu callback for host automation and parameter actions
    slider.onContextMenuRequested = [this]() {
      this->showContextMenuForSlider();
    };
  }

  /**
   * @brief Handles component resizing and lays out child widgets.
   *
   * @details
   * Calculates bounds for the slider and labels based on current scaling and
   * padding. Ensures all subcomponents are correctly positioned and sized.
   */
  inline void resized() override
  {
    TRACER("RotarySliderComponent::resized");
    const auto bounds = getLocalBounds();
    const auto padding = rawPadding * this->size;
    auto sliderBounds = bounds;
    const auto sliderSize =
      static_cast<int>(rawSliderSize * sliderBounds.getHeight());
#if DMT_EXCLUDE_SLIDER_GRAPHICS == 0
    slider.setBounds(sliderBounds.removeFromTop(sliderSize));
#endif
    const int labelPadding = static_cast<int>(padding * 0.5f);
    auto labelsBounds = bounds.reduced(labelPadding);
    const auto labelsSize =
      static_cast<int>(rawLabelsSize * labelsBounds.getHeight());
    labelsBounds = labelsBounds.removeFromBottom(labelsSize);
#if DMT_EXCLUDE_SLIDER_LABELS == 0
    this->titleLabel.setBounds(labelsBounds);
    this->infoLabel.setBounds(labelsBounds);
#endif
  }

  /**
   * @brief Paints debug bounds and center marker if enabled.
   *
   * @param _g The graphics context.
   *
   * @details
   * Only draws debug outlines and center marker if Settings::debugBounds is
   * true. All other painting is handled by subcomponents for maximum
   * flexibility.
   */
  inline void paint(juce::Graphics& _g) override
  {
    TRACER("RotarySliderComponent::paint");
    auto bounds = getLocalBounds();
    _g.setColour(juce::Colours::green);
    if (Settings::debugBounds) {
      _g.drawRect(bounds, 1);
      _g.setColour(juce::Colours::green);
      _g.drawEllipse(bounds.getCentreX(), bounds.getCentreY(), 5, 5, 1);
    }
  }

  /**
   * @brief Updates the info label when the slider value changes.
   *
   * @param _slider The slider that changed (unused).
   *
   * @details
   * This method is called by the slider listener.
   * It updates the info label to reflect the new value,
   * formatted with the appropriate unit.
   */
  inline void sliderValueChanged(juce::Slider* /*_slider*/) override
  {
    TRACER("RotarySliderComponent::sliderValueChanged");
    updateLabel(static_cast<float>(slider.getValue()));
  }

  /**
   * @brief Sets the size and center position of the component.
   *
   * @param _centrePoint The point to center the component on.
   *
   * @details
   * Calculates the size based on scaling and centers the component at the
   * specified point. Used for dynamic layout in parent containers.
   */
  inline void setSizeAndCentre(juce::Point<int> _centrePoint) noexcept
  {
    TRACER("RotarySliderComponent::setSizeAndCentre");
    const int width = static_cast<int>(baseWidth * this->size);
    const int height = static_cast<int>(baseHeight * this->size);
    setSize(width, height);
    setCentrePosition(_centrePoint.getX(), _centrePoint.getY());
  }

private:
  RotarySlider slider;
  SliderAttachment sliderAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotarySliderComponent)
};
//==============================================================================
} // namespace component
} // namespace gui
} // namespace dmt