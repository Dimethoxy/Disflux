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
 * MultiDisplay provides a container for multiple display components with
 * button-based switching and parameter change listening.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/display/AbstractDisplay.h"
#include "gui/widget/Shadow.h"
#include "gui/widget/SimpleButton.h"
#include "utility/Scaleable.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

/**
 * @class MultiDisplay
 * @brief Container for multiple display components with switching capability.
 */
class MultiDisplay
  : public juce::Component
  , public dmt::Scaleable<MultiDisplay>
  , public juce::AudioProcessorValueTreeState::Listener
{
  using String = juce::String;
  using APVTS = juce::AudioProcessorValueTreeState;
  using Display = dmt::gui::display::AbstractDisplay;
  using DisplayPtr = std::unique_ptr<Display>;
  using DisplayList = std::vector<DisplayPtr>;
  using SimpleButton = dmt::gui::widget::SimpleButton;
  using SimpleButtonPtr = std::unique_ptr<SimpleButton>;
  using SimpleButtonList = std::vector<SimpleButtonPtr>;
  using ParameterDisplayMap = std::unordered_map<juce::String, juce::String>;
  using Shadow = dmt::gui::widget::Shadow;
  using Image = juce::Image;
  using ImageComponent = juce::ImageComponent;

  //==============================================================================
  // Chrome settings (moved from AbstractDisplay)
  using DisplaySettings = dmt::Settings::Display;
  const juce::Colour& backgroundColour = DisplaySettings::backgroundColour;
  const juce::Colour& displayForegroundColour =
    dmt::Settings::Panel::backgroundColour;

  // Layout
  const float& rawCornerSize = DisplaySettings::cornerSize;
  const float& rawPadding = DisplaySettings::padding;

  // Border
  const bool& drawBorder = DisplaySettings::drawBorder;
  const juce::Colour& borderColour = DisplaySettings::borderColour;
  const float& rawBorderStrength = DisplaySettings::borderStrength;

  // Shadows
  const bool& drawOuterShadow = DisplaySettings::drawOuterShadow;
  const bool& drawInnerShadow = DisplaySettings::drawInnerShadow;
  const juce::Colour& outerShadowColour = DisplaySettings::outerShadowColour;
  const juce::Colour& innerShadowColour = DisplaySettings::innerShadowColour;
  const float& outerShadowRadius = DisplaySettings::outerShadowRadius;
  const float& innerShadowRadius = DisplaySettings::innerShadowRadius;

public:
  //==============================================================================
  explicit MultiDisplay(APVTS& _apvts,
                        DisplayList _displays,
                        ParameterDisplayMap _parameterDisplayMap = {})
    : apvts(_apvts)
    , outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true)
  {
    addAndMakeVisible(outerShadow);
    addAndMakeVisible(innerShadow);
    addAndMakeVisible(chromeComponent);
    setDisplays(std::move(_displays));
    fillButtonList();
    parameterDisplayMap = std::move(_parameterDisplayMap);
    addParameterListeners();
  }

  //============================================================================
  ~MultiDisplay() { removeParameterListeners(); }

  //==============================================================================
  void paint(juce::Graphics& _g) noexcept override
  {
    // Draw background for the entire multi-display area
    _g.setColour(backgroundColour);
    _g.fillAll();
  }

  //==============================================================================
  void resized() noexcept override
  {
    auto bounds = getLocalBounds();
    this->displayArea = bounds;

    // Update shadow paths and positions
    const auto padding = rawPadding * size;
    const auto borderStrength = rawBorderStrength * size;
    const auto cornerSize = rawCornerSize * size;
    const auto outerBounds =
      this->displayArea.reduced(static_cast<int>(padding));
    const auto innerBounds =
      outerBounds.reduced(static_cast<int>(borderStrength));
    const float outerCornerSize = cornerSize;
    const float innerCornerSize = std::clamp(
      outerCornerSize - (borderStrength * 0.5f), 0.0f, outerCornerSize);

    juce::Path outerShadowPath;
    outerShadowPath.addRoundedRectangle(outerBounds, outerCornerSize);
    outerShadow.setPath(outerShadowPath);
    outerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    outerShadow.toFront(false);

    juce::Path innerShadowPath;
    innerShadowPath.addRoundedRectangle(innerBounds, innerCornerSize);
    innerShadow.setPath(innerShadowPath);
    innerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    innerShadow.toBack();

    // Layout displays to fill the display area
    for (auto& display : displays) {
      display->setBounds(innerBounds);
    }

    // Layout buttons in the button area
    // for (auto& button : buttons) {
    //   button->setBounds(buttonArea.removeFromLeft(buttonSize));
    //   button->toFront(false);
    // }

    // Layout all displays to fill the area above the buttons
    // int rawButtonSize = 30;
    // int buttonSize = rawButtonSize * size;
    // int rawButtonPadding = 5;
    // int buttonPadding = rawButtonPadding * size;
    // auto buttonArea = bounds.removeFromBottom(buttonSize + 2 *
    // buttonPadding); buttonArea = buttonArea.reduced(buttonPadding, 0);

    drawChrome(innerBounds,
               outerBounds,
               innerCornerSize,
               outerCornerSize,
               borderStrength);
  }

  void drawChrome(const juce::Rectangle<int>& _innerBounds,
                  const juce::Rectangle<int>& _outerBounds,
                  float _innerCornerSize,
                  float _outerCornerSize,
                  float _borderStrength)
  {
    const auto bounds = getLocalBounds();
    const auto midBounds =
      juce::Rectangle<int>(_outerBounds).reduced(_borderStrength * 0.5f);
    const auto hiResWidth = static_cast<int>(bounds.getWidth() * scale);
    const auto hiResHeight = static_cast<int>(bounds.getHeight() * scale);

    // Prepare a high-resolution image for the chrome
    chrome = Image(Image::ARGB, hiResWidth, hiResHeight, true);
    juce::Graphics g(chrome);
    g.addTransform(juce::AffineTransform::scale(scale, scale));

    // First Pass: Draw the background
    if (drawBorder) {
      juce::Graphics::ScopedSaveState state(g);

      juce::Path fillClippingPath;
      fillClippingPath.setUsingNonZeroWinding(false);
      fillClippingPath.addRectangle(bounds.toFloat());
      fillClippingPath.addRoundedRectangle(midBounds.toFloat(),
                                           _innerCornerSize);
      g.reduceClipRegion(fillClippingPath);
      g.setColour(displayForegroundColour);
      g.fillRect(bounds);
    }

    // Second Pass: Draw the border
    {
      juce::Graphics::ScopedSaveState state(g);

      juce::Path borderClippingPath;
      borderClippingPath.setUsingNonZeroWinding(false);
      borderClippingPath.addRectangle(bounds.toFloat());
      borderClippingPath.addRoundedRectangle(_innerBounds.toFloat(),
                                             _innerCornerSize);
      g.reduceClipRegion(borderClippingPath);
      if (drawBorder) {
        g.setColour(borderColour);
        g.fillRoundedRectangle(_outerBounds.toFloat(), _outerCornerSize);
      } else {
        g.setColour(displayForegroundColour);
        g.fillRect(bounds);
      }
    }

    chromeComponent.setImage(chrome, juce::RectanglePlacement::stretchToFit);
    chromeComponent.setBounds(bounds);
    chromeComponent.toFront(false);
  }

  //==============================================================================
  void setActiveDisplay(size_t index)
  {
    if (index >= displays.size()) {
      jassertfalse; // Invalid index
      return;
    }

    // hide all displays
    for (auto& display : displays)
      display->setVisible(false);

    // set all buttons to active false
    for (auto& button : buttons)
      button->setToggleState(false, juce::dontSendNotification);

    // show the selected display
    displays[index]->setVisible(true);

    // set the corresponding button to the selected state
    buttons[index]->setToggleState(true, juce::dontSendNotification);
  }

protected:
  //==============================================================================
  void setDisplays(DisplayList _displays)
  {
    this->displays = std::move(_displays);

    for (auto& display : this->displays)
      // add child but don't make visible yet, as we only want to show display
      // 1
      addChildComponent(display.get());

    // make the first display visible
    if (!this->displays.empty())
      this->displays[0]->setVisible(true);
  }

  //==============================================================================
  void addParameterListeners()
  {
    for (const auto& [parameterID, displayName] : parameterDisplayMap) {
      apvts.addParameterListener(parameterID, this);
    }
  }

  //==============================================================================
  void removeParameterListeners()
  {
    for (const auto& [parameterID, displayName] : parameterDisplayMap) {
      apvts.removeParameterListener(parameterID, this);
    }
  }

  //==============================================================================
  void fillButtonList()
  {
    // clear existing buttons
    buttons.clear();

    // create a button for each display
    for (size_t i = 0; i < displays.size(); ++i) {
      String buttonNumber = String(i + 1);
      String buttonName = "Display" + buttonNumber + "Button";
      auto button = std::make_unique<SimpleButton>(
        buttonName, buttonNumber, "Switch to " + buttonName);
      button->onClick = [this, i]() { setActiveDisplay(i); };
      buttons.push_back(std::move(button));
      addAndMakeVisible(button.get());
    }
  }

private:
  //==============================================================================
  // AudioProcessorValueTreeState::Listener implementation
  void parameterChanged(const juce::String& parameterID,
                        float newValue) override
  {
    auto mappingIt = parameterDisplayMap.find(parameterID);

    // Validate we found something if not early exit
    if (mappingIt == parameterDisplayMap.end()) {
      jassertfalse; // No mapping found for this parameter ID
      return;
    }

    const auto& displayName = mappingIt->second;

    // Find the display index by name
    for (size_t i = 0; i < displays.size(); ++i) {
      if (displays[i]->getName() == displayName) {
        setActiveDisplay(i);
        return;
      }
    }

    // If we get here, we found a mapping we couldn't resolve to a display,
    // most likely the display is missing or there is a typo in its name or
    // the mapping
    jassertfalse;
    return;
  }

  //==============================================================================
  APVTS& apvts;
  DisplayList displays;
  SimpleButtonList buttons;
  ParameterDisplayMap parameterDisplayMap;
  juce::Rectangle<int> displayArea;
  Shadow outerShadow;
  Shadow innerShadow;
  Image chrome;
  ImageComponent chromeComponent;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiDisplay)
};

} // namespace display
} // namespace gui
} // namespace dmt