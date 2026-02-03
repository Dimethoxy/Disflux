/*
  ==============================================================================

    FolderManager.h
    Created: 28 Dec 2022 3:02:41pm
    Author:  Lunix

  ==============================================================================
*/

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