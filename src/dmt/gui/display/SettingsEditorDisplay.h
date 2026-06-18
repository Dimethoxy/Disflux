//==============================================================================
/* ██████╗ ██╗███╗   ███╗███████╗████████╗██╗  ██╗ ██████╗ ██╗  ██╗██╗   ██╗
 * ██╔══██╗██║████╗ ████║██╔════╝╚══██╔══╝██║  ██║██╔═══██╗╚██╗██╔╝╚██╗ ██╔╝
 * ██║  ██║██║██╔████╔██║█████╗     ██║   ███████║██║   ██║ ╚███╔╝  ╚████╔╝
 * ██║  ██║██║██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██║   ██║ ██╔██╗   ╚██╔╝
 * ██████╔╝██║██║ ╚═╝ ██║███████╗   ██║   ██║  ██║╚██████╔╝██╔╝ ██╗   ██║
 * ╚═════╝ ╚═╝╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * Part of the Dimethoxy Library, primarily intended for Dimethoxy plugins.
 * External use is permitted but not recommended.
 * No support or compatibility guarantees are provided.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * AbstractDisplay provides a base class for all display components
 * that require custom painting, shadow/border rendering, and repaint timing.
 * Designed for extensibility and real-time GUI performance.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/gui/component/SettingsEditorComponent.h"
#include "dmt/gui/display/DisplayChrome.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================
class SettingsEditorDisplay : public dmt::gui::display::DisplayChrome
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

  void resizeContent(
    const juce::Rectangle<int>& _contentBounds) noexcept override
  {
    TRACER("SettingsEditorDisplay::resizeContent");
    const auto padding = rawPadding * size;
    auto settingsBounds = _contentBounds.reduced(static_cast<int>(padding));
    settingsEditor.setBounds(settingsBounds);
  }

  virtual void paintDisplay(juce::Graphics& _g, juce::Rectangle<float> _bounds) {
    
  }

private:
  SettingsEditor settingsEditor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsEditorDisplay)
};

} // namespace display
} // namespace gui
} // namespace dmt
