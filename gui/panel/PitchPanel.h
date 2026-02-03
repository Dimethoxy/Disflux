//==============================================================================

#pragma once

#include "gui/panel/AbstractPanel.h"
#include "gui/panel/AnalogPitchPanel.h"
#include "gui/panel/Carousel.h"
#include "gui/panel/ModernPitchPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class PitchPanel : public dmt::gui::panel::Carousel
{
public:
  PitchPanel(juce::AudioProcessorValueTreeState& apvts)
    : Carousel()
  {
    panels.push_back(std::make_unique<AnalogPitchPanel>(apvts));
    panels.push_back(std::make_unique<ModernPitchPanel>());
    init();
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt
