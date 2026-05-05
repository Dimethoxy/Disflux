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
 * AnalogPitchPanel is a GUI component that provides controls for an analog
 * pitch envelope.
 *
 * Authors: Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/component/LinearSliderComponent.h"
#include "gui/panel/AbstractPanel.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace panel {

//==============================================================================
class AnalogPitchPanel : public dmt::gui::panel::AbstractPanel
{
  using LinearSliderComponent = dmt::gui::component::LinearSliderComponent;
  using LinearSliderType = dmt::gui::widget::LinearSlider::Type;
  using LinearSliderOrientation = dmt::gui::widget::LinearSlider::Orientation;
  using Unit = dmt::utility::Unit;

public:
  AnalogPitchPanel(juce::AudioProcessorValueTreeState& apvts)
    : AbstractPanel("Analog Pitch Envelope")
    , attackSlider(apvts,
                   juce::String("Attack"),
                   juce::String("osc1PitchEnvAttack"),
                   Unit::Type::Milliseconds,
                   LinearSliderType::Positive,
                   LinearSliderOrientation::Vertical)
    , depthSlider(apvts,
                  juce::String("Depth"),
                  juce::String("osc1PitchEnvDepth"),
                  Unit::Type::Frequency,
                  LinearSliderType::Positive,
                  LinearSliderOrientation::Vertical)
    , decaySlider(apvts,
                  juce::String("Decay"),
                  juce::String("osc1PitchEnvDecay"),
                  Unit::Type::Milliseconds,
                  LinearSliderType::Positive,
                  LinearSliderOrientation::Vertical)
    , bendSlider(apvts,
                 juce::String("Bend"),
                 juce::String("osc1PitchEnvBend"),
                 Unit::Type::EnvelopeBend,
                 LinearSliderType::Positive,
                 LinearSliderOrientation::Vertical)
  {
    setLayout({ 25, 32 });
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(depthSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(bendSlider);
  }

  void extendResize() noexcept override
  {
    const auto bounds = getLocalBounds();

    const int primaryRow = 3;
    const int secundaryRow = 28;

    const int attackCol = 7;
    const int decayCol = 11;
    const int bendCol = 15;
    const int depthCol = 19;

    const auto attackSliderPrimaryPoint =
      this->getGridPoint(bounds, attackCol, primaryRow);
    const auto attackSliderSecondaryPoint =
      this->getGridPoint(bounds, attackCol, secundaryRow);
    attackSlider.setBoundsByPoints(attackSliderPrimaryPoint,
                                   attackSliderSecondaryPoint);

    const auto decaySliderPrimaryPoint =
      this->getGridPoint(bounds, decayCol, primaryRow);
    const auto decaySliderSecondaryPoint =
      this->getGridPoint(bounds, decayCol, secundaryRow);
    decaySlider.setBoundsByPoints(decaySliderPrimaryPoint,
                                  decaySliderSecondaryPoint);

    const auto bendSliderPrimaryPoint =
      this->getGridPoint(bounds, bendCol, primaryRow);
    const auto bendSliderSecondaryPoint =
      this->getGridPoint(bounds, bendCol, secundaryRow);
    bendSlider.setBoundsByPoints(bendSliderPrimaryPoint,
                                 bendSliderSecondaryPoint);

    const auto depthSliderPrimaryPoint =
      this->getGridPoint(bounds, depthCol, primaryRow);
    const auto depthSliderSecondaryPoint =
      this->getGridPoint(bounds, depthCol, secundaryRow);
    depthSlider.setBoundsByPoints(depthSliderPrimaryPoint,
                                  depthSliderSecondaryPoint);
  }

private:
  LinearSliderComponent attackSlider;
  LinearSliderComponent depthSlider;
  LinearSliderComponent decaySlider;
  LinearSliderComponent bendSlider;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogPitchPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt
