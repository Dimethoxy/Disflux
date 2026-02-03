/*
  ==============================================================================

    FolderPanel.h
    Created: 28 Dec 2022 3:02:30pm
    Author:  Lunix

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace preset {
class FolderPanel
  : public juce::Component
  , juce::Button::Listener
  , juce::ComboBox::Listener
{
public:
  FolderPanel()
  {
    configureButton(newButton, "New");
    configureButton(previousFolderButton, "<");
    configureButton(nextFolderButton, ">");
    configureButton(deleteButton, "Delete");

    folderListBox.setTextWhenNothingSelected("No Folder Selected");
    folderListBox.setTextWhenNoChoicesAvailable("No Folder Available");
    folderListBox.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(folderListBox);
    folderListBox.addListener(this);
  }

  ~FolderPanel() override
  {
    newButton.removeListener(this);
    previousFolderButton.removeListener(this);
    nextFolderButton.removeListener(this);
    deleteButton.removeListener(this);
    folderListBox.removeListener(this);
  }

  void paint(juce::Graphics&) override {}

  void resized() override
  {
    auto padding = 1;
    const auto container = getLocalBounds().reduced(padding);
    auto bounds = container;

    newButton.setBounds(
      bounds.removeFromLeft(container.proportionOfWidth(0.2)).reduced(padding));
    previousFolderButton.setBounds(
      bounds.removeFromLeft(container.proportionOfWidth(0.1)).reduced(padding));
    folderListBox.setBounds(
      bounds.removeFromLeft(container.proportionOfWidth(0.4)).reduced(padding));
    nextFolderButton.setBounds(
      bounds.removeFromLeft(container.proportionOfWidth(0.1)).reduced(padding));
    deleteButton.setBounds(bounds.reduced(padding));
  }

private:
  void buttonClicked(juce::Button* button) override {}

  void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override {}

  void configureButton(juce::Button& button, const juce::String& buttontext)
  {
    button.setButtonText(buttontext);
    button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(button);
    button.addListener(this);
  }

  juce::TextButton newButton;
  juce::TextButton previousFolderButton;
  juce::TextButton nextFolderButton;
  juce::TextButton deleteButton;

  juce::ComboBox folderListBox;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FolderPanel)
};
} // namespace preset
} // namespace gui
} // namespace dmt