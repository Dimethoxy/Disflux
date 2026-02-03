/*
  ==============================================================================

                PresetPanel.h
                Created: 20 Dec 2022 10:29:23pm
                Author:  Lunix

                Source: https://youtu.be/YwAtWuGA4Cg

  ==============================================================================
*/

#pragma once

#include "PresetManager.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace preset {
class PresetPanel
  : public juce::Component
  , juce::Button::Listener
  , juce::ComboBox::Listener
{
public:
  PresetPanel(dmt::gui::preset::PresetManager& pm)
    : presetManager(pm)
  {
    configureButton(saveButton, "Save");
    configureButton(previousPresetButton, "<");
    configureButton(nextPresetButton, ">");
    configureButton(deleteButton, "Delete");

    presetListBox.setTextWhenNothingSelected("No Preset Selected");
    presetListBox.setTextWhenNoChoicesAvailable("No Presets Available");
    presetListBox.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(presetListBox);
    presetListBox.addListener(this);

    loadPresetList();
  }

  ~PresetPanel() override
  {
    saveButton.removeListener(this);
    previousPresetButton.removeListener(this);
    nextPresetButton.removeListener(this);
    deleteButton.removeListener(this);
    presetListBox.removeListener(this);
  }

  void paint(juce::Graphics&) override {}

  void resized() override
  {
    auto padding = 1;
    const auto container = getLocalBounds().reduced(padding);
    auto bounds = container;

    saveButton.setBounds(
      bounds.removeFromLeft(container.proportionOfWidth(0.2)).reduced(padding));
    previousPresetButton.setBounds(
      bounds.removeFromLeft(container.proportionOfWidth(0.1)).reduced(padding));
    presetListBox.setBounds(
      bounds.removeFromLeft(container.proportionOfWidth(0.4)).reduced(padding));
    nextPresetButton.setBounds(
      bounds.removeFromLeft(container.proportionOfWidth(0.1)).reduced(padding));
    deleteButton.setBounds(bounds.reduced(padding));
  }

private:
  void buttonClicked(juce::Button* button) override
  {
    if (button == &saveButton) {
      fileChooser = std::make_unique<juce::FileChooser>(
        "Save Preset",
        dmt::gui::preset::PresetManager::defaultDirectory,
        "*." + dmt::gui::preset::PresetManager::extension);

      fileChooser->launchAsync(juce::FileBrowserComponent::saveMode,
                               [&](const juce::FileChooser& chooser) {
                                 const auto result = chooser.getResult();
                                 presetManager.savePreset(
                                   result.getFileNameWithoutExtension());
                                 loadPresetList();
                               });
    }
    if (button == &previousPresetButton) {
      const auto index = presetManager.loadPreviousPreset();
      presetListBox.setSelectedItemIndex(index, juce::dontSendNotification);
    }
    if (button == &nextPresetButton) {
      const auto index = presetManager.loadNextPreset();
      presetListBox.setSelectedItemIndex(index, juce::dontSendNotification);
    }
    if (button == &deleteButton) {
      presetManager.deletePreset(presetManager.getCurrentPreset());
      loadPresetList();
    }
  }

  void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override
  {
    if (comboBoxThatHasChanged == &presetListBox) {
      presetManager.loadPreset(
        presetListBox.getItemText(presetListBox.getSelectedItemIndex()));
    }
  }

  void loadPresetList()
  {
    presetListBox.clear(juce::dontSendNotification);
    const auto allPresets = presetManager.getPresetList();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetListBox.addItemList(allPresets, 1);
    presetListBox.setSelectedItemIndex(allPresets.indexOf(currentPreset),
                                       juce::dontSendNotification);
  }

  void configureButton(juce::Button& button, const juce::String& buttontext)
  {
    button.setButtonText(buttontext);
    button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(button);
    button.addListener(this);
  }

  dmt::gui::preset::PresetManager presetManager;
  std::unique_ptr<juce::FileChooser> fileChooser;
  juce::TextButton saveButton;
  juce::TextButton previousPresetButton;
  juce::TextButton nextPresetButton;
  juce::TextButton deleteButton;

  juce::ComboBox presetListBox;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
};
} // namespace preset
} // namespace gui
} // namespace dmt