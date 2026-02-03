//==============================================================================

#pragma once

#include "gui/panel/AbstractPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class ModernPitchPanel : public dmt::gui::panel::AbstractPanel
{

public:
  ModernPitchPanel(/*juce::AudioProcessorValueTreeState& apvts*/)
    : AbstractPanel("Modern Pitch Envelope")
  {
    //
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernPitchPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt