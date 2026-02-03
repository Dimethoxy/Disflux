//==============================================================================

#pragma once

#include "gui/panel/AbstractPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class AnalogOscillatorPanel : public dmt::gui::panel::AbstractPanel
{
public:
  AnalogOscillatorPanel(/*juce::AudioProcessorValueTreeState& apvts*/)
    : AbstractPanel("Classic Oscillator")
  { //
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogOscillatorPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt
