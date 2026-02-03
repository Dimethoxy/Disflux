//==============================================================================

#pragma once

#include "gui/panel/AbstractPanel.h"
#include "gui/panel/AnalogGainPanel.h"
#include "gui/panel/Carousel.h"
#include "gui/panel/ModernGainPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class GainPanel : public dmt::gui::panel::Carousel
{
public:
  GainPanel(juce::AudioProcessorValueTreeState& apvts)
    : Carousel()
  {
    panels.push_back(std::make_unique<AnalogGainPanel>(apvts));
    panels.push_back(std::make_unique<ModernGainPanel>());
    init();
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt
