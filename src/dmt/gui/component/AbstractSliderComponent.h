//==============================================================================
/* ██████╗ ██╗███╗   ███╗███████╗████████╗██╗  ██╗ ██████╗ ██╗  ██╗██╗   ██╗
 * ██╔══██╗██║████╗ ████║██╔════╝╚══██╔══╝██║  ██║██╔═══██╗╚██╗██╔╝╚██╗ ██╔╝
 * ██║  ██║██║██╔████╔██║█████╗     ██║   ███████║██║   ██║ ╚███╔╝  ╚████╔╝
 * ██║  ██║██║██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██║   ██║ ██╔██╗   ╚██╔╝
 * ██████╔╝██║██║ ╚═╝ ██║███████╗   ██║   ██║  ██║╚██████╔╝██╔╝ ██╗   ██║
 * ╚═════╝ ╚═╝╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
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
 * AbstractSliderComponent provides a base class for slider components with
 * parameter binding, labels, and context menu support. It encapsulates common
 * logic for displaying the parameter value with units and showing the host
 * context menu.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/widget/Label.h"
#include "utility/Fonts.h"
#include "utility/HostContextMenu.h"
#include "utility/Settings.h"
#include "utility/Unit.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace component {

//==============================================================================
/**
 * @brief Abstract base class for slider components with parameter binding,
 * labels, and context menu.
 *
 * @details
 * Encapsulates common logic for parameter binding, title/info labels, unit
 * display, and context menu. Derived classes should implement layout, painting,
 * and slider instantiation.
 */
template<typename Derived, typename SliderType>
class AbstractSliderComponent
  : public juce::Component
  , public juce::Slider::Listener
  , public dmt::Scaleable<Derived>
  , public dmt::HostContextMenu<Derived>
{
protected:
  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  using Unit = dmt::utility::Unit;
  using Label = dmt::gui::widget::Label;
  using Fonts = dmt::utility::Fonts;
  using Settings = dmt::Settings;
  using SliderSettings = Settings::Slider;
  using RangedAudioParameter = juce::RangedAudioParameter;

  // Settings references
  const float& baseWidth = SliderSettings::baseWidth;
  const float& baseHeight = SliderSettings::baseHeight;
  const float& rawSliderSize = SliderSettings::sliderSize;
  const float& rawLabelsSize = SliderSettings::labelsSize;
  const float& rawPadding = SliderSettings::padding;
  const juce::Colour& titleFontColour = SliderSettings::titleFontColour;
  const juce::Colour& infoFontColour = SliderSettings::infoFontColour;
  const float& titleFontSize = SliderSettings::titleFontSize;
  const float& infoFontSize = SliderSettings::infoFontSize;

public:
  /**
   * @brief Constructs the abstract slider component.
   * @param _apvts Reference to the APVTS for parameter binding.
   * @param _text Title label text.
   * @param _param Parameter ID to bind.
   * @param _unitType Unit type for value display.
   */
  inline explicit AbstractSliderComponent(
    juce::AudioProcessorValueTreeState& _apvts,
    const juce::String& _text,
    const juce::String& _param,
    const Unit::Type _unitType)
    : titleLabel(_text, fonts.medium, titleFontSize, titleFontColour)
    , infoLabel(juce::String("Info Label"),
                fonts.light,
                infoFontSize,
                infoFontColour,
                juce::Justification::centredBottom)
    , unitType(_unitType)
    , fonts()
  {
    parameter = _apvts.getParameter(_param);
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(infoLabel);
  }

  /**
   * @brief Pointer to the attached parameter.
   */
  RangedAudioParameter* parameter = nullptr;

  /**
   * @brief Shows the context menu for the attached parameter.
   */
  inline void showContextMenuForSlider() noexcept
  {
    this->showContextMenu(parameter);
  }

  /**
   * @brief Updates the info label with the current slider value and unit.
   * @param value The value to display.
   */
  inline void updateLabel(float value) noexcept
  {
    auto text = Unit::getString(unitType, value);
    infoLabel.setText(text);
    infoLabel.repaint();
  }

protected:
  Label titleLabel;
  Label infoLabel;
  Unit::Type unitType;
  Fonts fonts;
};

} // namespace component
} // namespace gui
} // namespace dmt
