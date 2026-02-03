//==============================================================================

#pragma once

#include "gui/panel/AbstractPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class ModernOscillatorPanel : public dmt::gui::panel::AbstractPanel
{

public:
  ModernOscillatorPanel(/*juce::AudioProcessorValueTreeState& apvts*/)
    : AbstractPanel("Modern Oscillator")
  { //
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernOscillatorPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt