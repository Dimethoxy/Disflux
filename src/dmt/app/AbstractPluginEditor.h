#pragma once

#include "./AbstractPluginProcessor.h"
#include <JuceHeader.h>

namespace dmt {
namespace app {
class AbstractPluginEditor : public juce::AudioProcessorEditor
{
public:
  explicit AbstractPluginEditor(dmt::app::AbstractPluginProcessor& p)
    : juce::AudioProcessorEditor(&p)
  {
  }

  ~AbstractPluginEditor() override = default;
};
} // namespace app
} // namespace dmt