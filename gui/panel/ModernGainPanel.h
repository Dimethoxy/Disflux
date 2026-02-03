//==============================================================================

#pragma once

#include "gui/panel/AbstractPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class ModernGainPanel : public dmt::gui::panel::AbstractPanel
{

public:
  ModernGainPanel(/*juce::AudioProcessorValueTreeState& apvts*/)
    : AbstractPanel("Modern Gain Envelope")
  {
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernGainPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt