#pragma once

#include "PluginProcessor.h"
#include <DmtHeader.h>

//==============================================================================
class PluginEditor : public dmt::app::AbstractPluginEditor
{
public:
  explicit PluginEditor(PluginProcessor&);
  ~PluginEditor() override;

private:
  //==============================================================================

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
