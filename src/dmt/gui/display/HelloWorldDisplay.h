#pragma once

//==============================================================================

#include "gui/display/AbstractDisplay.h"
#include "gui/widget/Label.h"
// #include "utility/Settings.h"
#include "utility/Fonts.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

/**
 * @class DisfluxDisplay
 * @brief Specialized display component inheriting from
 * AbstractDisplay.
 */
class HelloWorldDisplay : public dmt::gui::display::AbstractDisplay
{
  using Label = dmt::gui::widget::Label;
  using Settings = dmt::Settings;
  using Fonts = dmt::utility::Fonts;
  using Colour = juce::Colour;

public:
  explicit HelloWorldDisplay()
    : AbstractDisplay()
    , label("Hello, World!", fonts.bold, rawFontSize, juce::Colours::white)
  {
    addAndMakeVisible(label);
  }

  void resized() noexcept override { label.setBounds(getLocalBounds()); }

  void paint(juce::Graphics& _g) noexcept override
  {
    if (Settings::debugBounds) {
      _g.setColour(juce::Colours::cyan);
      _g.drawRect(getLocalBounds(), 1);
    }
  }

private:
  Label label;
  Fonts fonts;
  float rawFontSize = 24.0f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelloWorldDisplay)
};
} // namespace component
} // namespace gui
} // namespace dmt