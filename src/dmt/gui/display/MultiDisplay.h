// src/dmt/gui/widget/SimpleButton.h
//==============================================================================
#pragma once

#include "gui/display/AbstractDisplay.h"
#include "gui/widget/SimpleButton.h"
#include "utility/Scaleable.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

namespace dmt {
namespace gui {
namespace display {

class MultiDisplay : public dmt::gui::display::AbstractDisplay
{

  using Display = dmt::gui::display::AbstractDisplay;
  using DisplayPtr = std::unique_ptr<Display>;
  using DisplayList = std::vector<DisplayPtr>;
  using SimpleButton = dmt::gui::widget::SimpleButton;
  using SimpleButtonPtr = std::unique_ptr<SimpleButton>;
  using SimpleButtonList = std::vector<SimpleButtonPtr>;

public:
  explicit MultiDisplay(DisplayList _displays)
    : AbstractDisplay(/*_apvts*/)
  {
    setDisplays(std::move(_displays));
  }

  void extendResized(
    const juce::Rectangle<int>& _displayBounds) noexcept override
  {
    auto& bounds = _displayBounds;

    int rawButtonSize = 30;
    int buttonSize = rawButtonSize * size;
    int rawButtonPadding = 5;
    int buttonPadding = rawButtonPadding * size;
    auto buttonArea = juce::Rectangle<int>(bounds)
                        .removeFromBottom(buttonSize + 2 * buttonPadding)
                        .reduced(buttonPadding);
    for (auto& button : buttons) {
      button->setBounds(buttonArea.removeFromLeft(buttonSize));
    }

    // layout all displays to fill the max are
    for (auto& display : displays) {
      display->setBounds(bounds);
    }
  }

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
  void setDisplays(DisplayList _displays)
  {
    this->displays = std::move(_displays);

    for (auto& display : this->displays)
      // add child but don't make visible yet, as we only want to show display 1
      addChildComponent(display.get());

    // make the first display visible
    if (!this->displays.empty())
      this->displays[0]->setVisible(true);

    fillButtonList();
  }

  void fillButtonList()
  {
    // clear existing buttons
    buttons.clear();

    // create a button for each display
    for (size_t i = 0; i < displays.size(); ++i) {
      String buttonNumber = juce::String(i + 1);
      String buttonName = "Display" + buttonNumber + "Button";
      auto button = std::make_unique<SimpleButton>(
        buttonName, buttonNumber, "Switch to " + buttonName);
      button->onClick = [this, i]() { setActiveDisplay(i); };
      buttons.push_back(std::move(button));
    }
  }

private:
  DisplayList displays;
  SimpleButtonList buttons;
};
} // namespace display
} // namespace gui
} // namespace dmt