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
 * DisfluxPanel is the main panel for the Disflux effect.
 * It also comes with an oscilloscope display.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

// Define to 1 to exclude DisfluxDisplay from the build
#define DMT_EXCLUDE_DISFLUX_DISPLAY 0

//==============================================================================

#include "dsp/data/FifoAudioBuffer.h"
#include "gui/component/LinearSliderComponent.h"
#include "gui/component/RotarySliderComponent.h"
#if DMT_EXCLUDE_DISFLUX_DISPLAY == 0
#include "gui/display/DisfluxDisplay.h"
#endif
#include "gui/panel/AbstractPanel.h"
#include "utility/Settings.h"
#include "utility/Unit.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace panel {

//==============================================================================
/**
 * @brief Panel for Disflux oscilloscope display and parameter controls.
 *
 * @tparam SampleType The floating-point sample type for audio data.
 *
 * @details
 * This class provides a GUI panel for the Disflux effect, including an
 * oscilloscope display and various parameter sliders. It inherits from
 * AbstractPanel and uses a grid layout for positioning components.
 */
template<typename SampleType>
class DisfluxPanel : public dmt::gui::panel::AbstractPanel
{
  using FifoAudioBuffer = dmt::dsp::data::FifoAudioBuffer<float>;
#if DMT_EXCLUDE_DISFLUX_DISPLAY == 0
  using DisfluxDisplay = dmt::gui::display::DisfluxDisplay;
#endif
  using LinearSlider = dmt::gui::component::LinearSliderComponent;
  using LinearSliderType = dmt::gui::widget::LinearSlider::Type;
  using LinearSliderOrientation = dmt::gui::widget::LinearSlider::Orientation;
  using RotarySlider = dmt::gui::component::RotarySliderComponent;
  using RotarySliderType = dmt::gui::widget::RotarySlider::Type;
  using Unit = dmt::utility::Unit;
  using Settings = dmt::Settings;
  const float& rawPadding = Settings::Panel::padding;

public:
  //==============================================================================
  /**
   * @brief Constructs a DisfluxPanel.
   *
   * @param _apvts The AudioProcessorValueTreeState for parameter binding.
   * @param _oscilloscopeBuffer The FIFO buffer for oscilloscope display.
   *
   * @details
   * Initializes all display and slider components, sets up the grid layout,
   * and adds all subcomponents to the panel.
   */
  constexpr inline explicit DisfluxPanel(
    juce::AudioProcessorValueTreeState& _apvts,
    FifoAudioBuffer& _oscilloscopeBuffer) noexcept
    : AbstractPanel("Oscilloscope", false)
#if DMT_EXCLUDE_DISFLUX_DISPLAY == 0
    , display(_oscilloscopeBuffer, _apvts)
#endif
    , amountSlider(_apvts,
                   juce::String("Amount"),
                   juce::String("DisfluxAmount"),
                   Unit::Type::DisfluxAmount,
                   RotarySliderType::Positive)
    , spreadSlider(_apvts,
                   juce::String("Spread"),
                   juce::String("DisfluxSpread"),
                   Unit::Type::DisfluxSpread,
                   RotarySliderType::Positive)
    , fequencySlider(_apvts,
                     juce::String("Frequency"),
                     juce::String("DisfluxFrequency"),
                     Unit::Type::DisfluxFrequency,
                     LinearSliderType::Positive,
                     LinearSliderOrientation::Horizontal)
    , pinchSlider(_apvts,
                  juce::String("Pinch"),
                  juce::String("DisfluxPinch"),
                  Unit::Type::DisfluxPinch,
                  RotarySliderType::Positive)
    , mixSlider(_apvts,
                juce::String("Mix"),
                juce::String("DisfluxMix"),
                Unit::Type::DisfluxMix,
                RotarySliderType::Positive)
  {
    TRACER("DisfluxPanel::DisfluxPanel");
    setLayout({ 22, 60 });
#if DMT_EXCLUDE_DISFLUX_DISPLAY == 0
    addAndMakeVisible(display);
#endif
    addAndMakeVisible(amountSlider);
    addAndMakeVisible(spreadSlider);
    addAndMakeVisible(fequencySlider);
    addAndMakeVisible(pinchSlider);
    addAndMakeVisible(mixSlider);
  }

  //==============================================================================
  /**
   * @brief Handles resizing and layout of all subcomponents.
   *
   * @details
   * Positions the oscilloscope display and all sliders using grid points,
   * ensuring consistent layout regardless of panel size.
   * Override from AbstractPanel.
   */
  inline void extendResize() noexcept override
  {
    TRACER("DisfluxPanel::extendResize");
    auto bounds = getLocalBounds();

    const float padding = rawPadding * size;
    auto displayBounds = bounds.reduced(padding);
    const float displayHorizontalPadding = 100.0f;
    const float displayVerticalPadding = 57.0f;
    displayBounds.removeFromBottom(displayVerticalPadding * size);
    displayBounds.removeFromLeft(displayHorizontalPadding * size);
    displayBounds.removeFromRight(displayHorizontalPadding * size);
#if DMT_EXCLUDE_DISFLUX_DISPLAY == 0
    display.setBounds(displayBounds);
#endif

    const int upperRotarySliderRow = 17;
    const int lowerRotarySliderRow = 43;
    const int linearSliderRow = 51;

    const int amountSliderCol = 3;
    const int leftFequencySliderCol = 6;
    const int rightFequencySliderCol = 17;
    const int pinchSliderCol = 20;

    const auto amountSliderPoint =
      this->getGridPoint(bounds, amountSliderCol, upperRotarySliderRow);
    amountSlider.setSizeAndCentre(amountSliderPoint);

    const auto spreadSliderPoint =
      this->getGridPoint(bounds, amountSliderCol, lowerRotarySliderRow);
    spreadSlider.setSizeAndCentre(spreadSliderPoint);

    const auto leftFequencySliderPoint =
      this->getGridPoint(bounds, leftFequencySliderCol, linearSliderRow);
    const auto rightFequencySliderPoint =
      this->getGridPoint(bounds, rightFequencySliderCol, linearSliderRow);
    fequencySlider.setBoundsByPoints(leftFequencySliderPoint,
                                     rightFequencySliderPoint);

    const auto pinchSliderPoint =
      this->getGridPoint(bounds, pinchSliderCol, lowerRotarySliderRow);
    pinchSlider.setSizeAndCentre(pinchSliderPoint);

    const auto mixSliderPoint =
      this->getGridPoint(bounds, pinchSliderCol, upperRotarySliderRow);
    mixSlider.setSizeAndCentre(mixSliderPoint);
  }
  //==============================================================================

private:
  //==============================================================================
  // Members initialized in the initializer list
#if DMT_EXCLUDE_DISFLUX_DISPLAY == 0
  DisfluxDisplay display;
#endif
  RotarySlider amountSlider;
  RotarySlider spreadSlider;
  LinearSlider fequencySlider;
  RotarySlider pinchSlider;
  RotarySlider mixSlider;

  //==============================================================================
  // Other members

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisfluxPanel)
};

} // namespace panel
} // namespace gui
} // namespace dmt