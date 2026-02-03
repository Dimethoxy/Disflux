//==============================================================================

#pragma once

//==============================================================================

#include "dmt/gui/component/SettingsEditorComponent.h"
#include "dmt/gui/display/AbstractDisplay.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {
// TODO: Make this use the new display system with multithreading
//==============================================================================
class SettingsEditorDisplay : public dmt::gui::display::AbstractDisplay
{
  using Settings = dmt::Settings;
  using SettingsEditorSettings = dmt::Settings::SettingsEditor;
  using TextEditor = dmt::gui::widget::TextEditor;
  using SettingsEditor = dmt::gui::component::SettingsEditor;

  //==============================================================================
  // SettingsEditor
  const float& rawPadding = SettingsEditorSettings::padding;

public:
  SettingsEditorDisplay()
  {
    TRACER("SettingsEditorDisplay::SettingsEditorDisplay");
    addAndMakeVisible(settingsEditor);
  }

  ~SettingsEditorDisplay() override = default;

  void extendResized(
    const juce::Rectangle<int>& _displayBounds) noexcept override
  {
    TRACER("SettingsEditorDisplay::extendResized");
    const auto padding = rawPadding * size;
    auto settingsBounds = _displayBounds.reduced(padding);
    settingsEditor.setBounds(settingsBounds);
  }

  void paintDisplay(juce::Graphics& /*_g*/,
                    const juce::Rectangle<int>& /*_displayBounds*/) noexcept
  {
    TRACER("SettingsEditorDisplay::paintDisplay");
    if (cachedPadding != rawPadding) {
      cachedPadding = rawPadding;
      resized();
    }
  }

  void prepareNextFrame() noexcept override
  {
    TRACER("SettingsEditorDisplay::prepareNextFrame");
    // Implement frame preparation logic here
  }

private:
  SettingsEditor settingsEditor;
  float cachedPadding = 0.0f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsEditorDisplay)
};

} // namespace component
} // namespace gui
} // namespace dmt