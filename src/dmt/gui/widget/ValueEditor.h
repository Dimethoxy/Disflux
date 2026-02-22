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
 * ValueEditor is a GUI component that provides an interface for editing a
 * single value from the configuration. It consists of a label and a text
 * editor. The label displays the name of the value, and the text editor allows
 * the user to edit the value. When the user finishes editing (by losing focus
 * or pressing  Enter), the new value is parsed and set in the configuration. If
 * the new value is invalid, it reverts to the previous value. It also notifies
 * a listener (if one exists) about the change, allowing for UI updates or other
 * reactions.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/gui/widget/Label.h"
#include "dmt/gui/widget/TextEditor.h"
#include "dmt/utility/Fonts.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace component {
//==============================================================================
class ValueEditor
  : public juce::Component
  , public dmt::Scaleable<ValueEditor>
{
  using Colour = juce::Colour;
  using Fonts = dmt::utility::Fonts;
  using Label = dmt::gui::widget::Label;
  using Settings = dmt::Settings;
  using String = juce::String;
  using TextEditor = dmt::gui::widget::TextEditor;
  using TreeAdapter = dmt::configuration::TreeAdapter;
  using SettingsEditorSettings = dmt::Settings::SettingsEditor;

  //==============================================================================
  // SettingsEditor
  const float& labelHorizontalPadding =
    SettingsEditorSettings::labelHorizontalPadding;
  const float& fontSize = SettingsEditorSettings::fontSize;
  const Colour& fontColour = SettingsEditorSettings::fontColour;

public:
  class Listener
  {
  public:
    virtual void valueEditorListenerCallback() = 0;
  };

public:
  ValueEditor(TreeAdapter::Leaf& _leaf)
    : leaf(_leaf)
    , label(String(leaf.name),
            fonts.medium,
            fontSize,
            fontColour,
            juce::Justification::centredLeft)
    , editor(String(leaf.name + "Editor"))
  {
    TRACER("ValueEditor::ValueEditor");
    addAndMakeVisible(label);
    addAndMakeVisible(editor);

    editor.setText(String(leaf.toString()));
    setStyle();

    editor.onFocusLost = [this]() { newValueCallback(); };
    editor.onReturnKey = [this]() { newValueCallback(); };

    editor.setWantsKeyboardFocus(true);
    editor.setMouseClickGrabsKeyboardFocus(true);
    editor.setEscapeAndReturnKeysConsumed(true);
  }

  ~ValueEditor() override = default;

  void paint(juce::Graphics& _g) override
  {
    TRACER("ValueEditor::paint");
    if (!editor.hasKeyboardFocus(false)) {
      editor.setText(leaf.toString(), juce::dontSendNotification);
    }
  }

  void resized() override
  {
    TRACER("ValueEditor::resized");
    auto bounds = getLocalBounds();
    auto labelBounds = bounds.removeFromLeft(bounds.getWidth() / 3.0f * 2.0f);
    auto editorBounds = bounds;
    label.setBounds(labelBounds);
    editor.setBounds(editorBounds);
    editor.setFont(fonts.medium.withHeight(fontSize * size));
    editor.setText("", juce::dontSendNotification);
    editor.setText(leaf.toString(), juce::dontSendNotification);
    editor.setWantsKeyboardFocus(true);
    editor.setMouseClickGrabsKeyboardFocus(true);
    editor.setEscapeAndReturnKeysConsumed(true);
  }

  // TODO: This is absolutely horrible
  void setStyle()
  {
    TRACER("ValueEditor::setStyle");
    label.setRawHorizontalPadding(labelHorizontalPadding);
    editor.setColour(juce::TextEditor::textColourId, fontColour);
    editor.setColour(juce::TextEditor::backgroundColourId,
                     juce::Colours::transparentBlack);
    editor.setColour(juce::TextEditor::outlineColourId,
                     juce::Colours::transparentBlack);
  }

  void newValueCallback()
  {
    TRACER("ValueEditor::newValueCallback");
    auto newText = editor.getText();
    if (!leaf.parseAndSet(newText)) {
      editor.setText(leaf.toString(), juce::dontSendNotification);
      return;
    }
    auto* parent = getParentComponent();
    while (dynamic_cast<Listener*>(parent) == nullptr) {
      parent = parent->getParentComponent();
      if (parent == nullptr) {
        break;
      }
    }
    if (parent != nullptr) {
      Listener* listener = dynamic_cast<Listener*>(parent);
      listener->valueEditorListenerCallback();
    }

    auto* topLevelComponent = getTopLevelComponent();
    topLevelComponent->repaint();
    editor.setWantsKeyboardFocus(true);
    editor.setMouseClickGrabsKeyboardFocus(true);
    editor.setEscapeAndReturnKeysConsumed(true);
  }

  TextEditor& getEditor() { return editor; }

private:
  //==============================================================================
  TreeAdapter::Leaf& leaf;
  Label label;
  TextEditor editor;

private:
  //==============================================================================
  Fonts fonts;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValueEditor)
};

} // namespace component
} // namespace gui
} // namespace dmt