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

  void resized() noexcept override
  {
    TRACER("SettingsEditorDisplay::resized");
    const auto padding = rawPadding * size;
    auto settingsBounds = getLocalBounds().reduced(static_cast<int>(padding));
    settingsEditor.setBounds(settingsBounds);
  }

  void paint(juce::Graphics& /*_g*/) noexcept override
  {
    TRACER("SettingsEditorDisplay::paint");
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