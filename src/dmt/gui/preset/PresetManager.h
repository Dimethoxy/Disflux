/*
  ==============================================================================

        PresetManager.h
        Created: 20 Dec 2022 11:46:29pm
        Author:  Lunix

        Source: https://youtu.be/YwAtWuGA4Cg

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace preset {
class PresetManager : juce::ValueTree::Listener
{
public:
  inline static const juce::File& defaultDirectory =
    juce::File::getSpecialLocation(
      juce::File::SpecialLocationType::userDocumentsDirectory)
      .getChildFile(ProjectInfo::companyName)
      .getChildFile(ProjectInfo::projectName);

  inline static const juce::String& extension = "preset";
  inline static const juce::String& presetNameProperty = "presetName";

  //==============================================================================
  PresetManager(juce::AudioProcessorValueTreeState& apvts)
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
    currentPreset.referTo(
      valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
  }

  //==============================================================================
  void savePreset(const juce::String& presetName)
  {
    if (presetName.isEmpty())
      return;
    currentPreset.setValue(presetName);
    const auto xml = valueTreeState.copyState().createXml();
    const auto presetFile =
      defaultDirectory.getChildFile(presetName + "." + extension);
    if (!xml->writeTo(presetFile)) {
      DBG("Could not create preset file: " + presetFile.getFullPathName());
      jassertfalse;
    }
  }

  void loadPreset(const juce::String& presetName)
  {
    if (presetName.isEmpty())
      return;

    const auto presetFile =
      defaultDirectory.getChildFile(presetName + "." + extension);
    if (!presetFile.existsAsFile()) {
      DBG("Preset file does not exist: " + presetFile.getFullPathName());
      jassertfalse;
      return;
    }

    juce::XmlDocument xmlDocument{ presetFile };
    const auto valueTreeToLoad =
      juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());
    for (size_t i = 0; i < valueTreeToLoad.getNumChildren(); i++) {
      const auto parameterChildToLoad = valueTreeToLoad.getChild(i);
      const auto parameterId = parameterChildToLoad.getProperty("id");
      auto parameterTree =
        valueTreeState.state.getChildWithProperty("id", parameterId);
      if (parameterTree.isValid()) {
        parameterTree.copyPropertiesFrom(parameterChildToLoad, nullptr);
      }
    }
    currentPreset.setValue(presetName);
  }

  void loadInitPreset()
  {
    // valueTreeState.
  }

  void deletePreset(const juce::String& presetName)
  {
    if (presetName.isEmpty())
      return;

    const auto presetFile =
      defaultDirectory.getChildFile(presetName + "." + extension);
    if (!presetFile.existsAsFile()) {
      DBG("Preset file does not exist: " + presetFile.getFullPathName());
      jassertfalse;
      return;
    }
    if (!presetFile.deleteFile()) {
      DBG("Could not delete preset file: " + presetFile.getFullPathName());
      jassertfalse;
      return;
    }
    currentPreset = "";
  }

  //==============================================================================
  int loadNextPreset()
  {
    const auto presetList = getPresetList();
    if (presetList.isEmpty())
      return -1;
    const auto currentIndex = presetList.indexOf(currentPreset.toString());
    const auto nextIndex =
      (currentIndex == (presetList.size() - 1)) ? 0 : currentIndex + 1;
    loadPreset(presetList[nextIndex]);
    return nextIndex;
  }

  int loadPreviousPreset()
  {
    const auto presetList = getPresetList();
    if (presetList.isEmpty())
      return -1;
    const auto currentIndex = presetList.indexOf(currentPreset.toString());
    const auto previousIndex =
      (currentIndex == 0) ? presetList.size() - 1 : currentIndex - 1;
    loadPreset(presetList[previousIndex]);
    return previousIndex;
  }

  //==============================================================================
  juce::StringArray getPresetList()
  {
    juce::StringArray presetList;
    auto fileArray = defaultDirectory.findChildFiles(
      juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);
    for (const auto& file : fileArray) {
      presetList.add(file.getFileNameWithoutExtension());
    }
    return presetList;
  }

  juce::String getCurrentPreset() const { return currentPreset.toString(); }

  void valueTreeRedirected(
    juce::ValueTree& /*treeWhichHasBeenChanged*/) override
  {
    currentPreset.referTo(
      valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
  }

private:
  //==============================================================================
  juce::AudioProcessorValueTreeState& valueTreeState;
  juce::Value currentPreset;
};
} // namespace preset
} // namespace gui
} // namespace dmt
