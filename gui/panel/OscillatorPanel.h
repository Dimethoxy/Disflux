//==============================================================================

#pragma once

#include "gui/panel/AbstractPanel.h"
#include "gui/panel/AnalogOscillatorPanel.h"
#include "gui/panel/Carousel.h"
#include "gui/panel/ModernOscillatorPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class OscillatorPanel : public dmt::gui::panel::Carousel
{
public:
  OscillatorPanel()
    : Carousel()
  {
    panels.push_back(std::make_unique<AnalogOscillatorPanel>());
    panels.push_back(std::make_unique<ModernOscillatorPanel>());
    init();
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt
