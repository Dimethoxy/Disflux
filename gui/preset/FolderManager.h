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
 * FolderManager is responsible for managing preset folders within the plugin's
 * preset system. It listens to changes in the AudioProcessorValueTreeState to
 * update the current folder and maintain a list of available folders. It also
 * ensures that a default directory for presets exists and is accessible.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace preset {
class FolderManager : juce::ValueTree::Listener
{
public:
  inline static const juce::File& defaultDirectory =
    juce::File::getSpecialLocation(
      juce::File::SpecialLocationType::commonDocumentsDirectory)
      .getChildFile(ProjectInfo::companyName)
      .getChildFile(ProjectInfo::projectName);

  inline static const juce::String& folderNameProperty = "folderName";

  FolderManager(juce::AudioProcessorValueTreeState& apvts)
    : valueTreeState(apvts)
  {
    // Create default directory
    if (!defaultDirectory.exists()) {
      const auto result = defaultDirectory.createDirectory();
      if (result.failed()) {
        DBG("Could not create preset directory: " + result.getErrorMessage());
        jassertfalse;
      }
    }
    valueTreeState.state.addListener(this);
    currentFolder.referTo(
      valueTreeState.state.getPropertyAsValue(folderNameProperty, nullptr));
  }

private:
  //==============================================================================
  juce::AudioProcessorValueTreeState& valueTreeState;
  juce::StringArray folderList;
  juce::Value currentFolder;
};
} // namespace preset
} // namespace gui
} // namespace dmt