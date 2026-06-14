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
 * SimpleButton is more lightweight than AbstractButton, designed for use cases
 * where shadows and background states are not needed. It still supports icons
 * and tooltips, making it ideal for minimalist UI elements.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/widget/Shadow.h"
#include "utility/Icon.h"
#include "utility/Scaleable.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

class SimpleButton
  : public juce::Button
  , public dmt::Scaleable<SimpleButton>
{
  using String = juce::String;
  using Settings = dmt::Settings;
  using Colour = juce::Colour;
  using Image = juce::Image;

  const Colour& backgroundColour = Settings::Display::backgroundColour;

public:
  SimpleButton(String _name, String _displayString, String _tooltip = "")
    : juce::Button(_name)
    , tooltip(_tooltip)
  {
    setButtonText(_displayString);
  }

  void resized() override
  {
    // No custom resizing logic needed for SimpleButton
  }

  void setActiveState()
  {
    this->isActive = true;
    repaint();
  }

  void setPassiveState()
  {
    this->isActive = false;
    repaint();
  }

  void paintButton(juce::Graphics& g,
                   bool isMouseOverButton,
                   bool isButtonDown) override
  {

    const float borderThickness = 1.0f * size;
    const float borderRadius = 3.0f * size;
    const auto outerBounds = getLocalBounds().toFloat();
    const auto innerBounds = outerBounds.reduced(borderThickness);

    if (isButtonDown) {
      g.setColour(juce::Colours::white);
      g.fillRect(innerBounds);
    } else if (isActive) {
      g.fillAll(juce::Colours::white);
    } else {
      g.fillAll(backgroundColour);
      g.setColour(juce::Colours::white);
      g.drawRect(outerBounds, borderThickness);
    }

    g.fillAll(juce::Colours::red);
  }

  //==============================================================================
  /**
   * @brief Retrieves the tooltip text for the button.
   * @return The tooltip text as a String.
   *
   * @details Used by JUCE's tooltip system to display contextual help.
   */
  [[nodiscard]] inline String getTooltip() override
  {
    TRACER("SimpleButton::getTooltip");
    return tooltip;
  }

private:
  String tooltip;
  bool isActive = false;
};

} // namespace widget
} // namespace gui
} // namespace dmt
