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
 * SettingsEditorComponent provides a user interface for editing application
 * settings in a structured and user-friendly way. It displays settings
 * categories in a scrollable list on the left, and when a category is selected,
 * it shows the corresponding settings editors on the right. The component
 * supports dynamic resizing, customizable scrollbars, and is designed to be
 * easily extendable for different types of settings.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/configuration/TreeAdapter.h"
#include "dmt/gui/widget/ValueCategoryList.h"
#include "dmt/gui/widget/ValueEditorList.h"
#include "dmt/utility/Scaleable.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace component {
//==============================================================================
class SettingsEditor
  : public juce::Component
  , public dmt::Scaleable<SettingsEditor>
{
  using Colour = juce::Colour;
  using Settings = dmt::Settings;
  using SettingsEditorSettings = dmt::Settings::SettingsEditor;
  using String = juce::String;
  using Viewport = juce::Viewport;
  using TreeAdapter = dmt::configuration::TreeAdapter;
  using ValueEditorList = dmt::gui::component::ValueEditorList;
  using ValueCategoryList = dmt::gui::component::ValueCategoryList;
  using Component = juce::Component;

  //==============================================================================
  // SettingsEditor
  const Colour& scrollBarColour = SettingsEditorSettings::scrollBarColour;
  const float rawScrollBarThickness =
    SettingsEditorSettings::scrollBarThickness;
  const Colour& scrollBarBackgroundColour =
    SettingsEditorSettings::scrollBarBackgroundColour;
  const float& rawFontSize = SettingsEditorSettings::fontSize;

  // TODO: Move somewhere else
  std::vector<juce::String> blockedCategories = { "TriangleButton",
                                                  "OscillatorDisplay",
                                                  "Carousel" };

public:
  SettingsEditor()
    : treeAdapter(Settings::container, blockedCategories)
    , searchEditor("TestEditor")
    , valueCategoryList(treeAdapter.getCategories(),
                        [this](TreeAdapter::Category& category) {
                          onCategorySelectedCallback(category);
                        })

  {
    TRACER("SettingsEditor::SettingsEditor");
    addAndMakeVisible(searchEditor);
    addAndMakeVisible(categoryViewport);
    addAndMakeVisible(editorViewport);

    categoryViewport.setViewedComponent(&valueCategoryList, false);
    categoryViewport.setScrollBarsShown(false, false, false, false);
    editorViewport.setViewedComponent(&valueEditorList, false);
    editorViewport.setScrollBarsShown(true, false, false, false);
    setScrollbarThicknesses();
    setScrollBarColour();
  }

  ~SettingsEditor() override = default;

  void paint(juce::Graphics& /*_g*/) override
  {
    TRACER("SettingsEditor::paint");
  }

  void resized() override
  {
    TRACER("SettingsEditor::resized");
    auto bounds = getLocalBounds();

    // Set bounds for the search editor
    // const auto fontSize = static_cast<int>(rawFontSize * size);
    // searchEditor.setBounds(bounds.removeFromTop(fontSize));

    // Calculate and set bounds for the category viewport
    const int categoryWidth = static_cast<int>(bounds.getWidth() * 0.35f);
    categoryViewport.setBounds(bounds.removeFromLeft(categoryWidth));
    layoutViewport(categoryViewport, valueCategoryList);

    // Set bounds for the editor viewport
    editorViewport.setBounds(bounds);
    layoutViewport(editorViewport, valueEditorList);

    // Update scrollbar thicknesses
    setScrollbarThicknesses();
  }

  void onCategorySelectedCallback(TreeAdapter::Category& category)
  {
    TRACER("SettingsEditor::onCategorySelectedCallback");
    std::cout << "Selected category: " << category.name << std::endl;
    valueEditorList.setCategory(category);
    valueEditorList.setOptimalSize(editorViewport.getWidth());
  }

protected:
  template<typename ComponentType>
  void layoutViewport(Viewport& viewport, ComponentType& content)
  {
    TRACER("SettingsEditor::layoutViewport");
    const int optimalWidth =
      viewport.isVerticalScrollBarShown()
        ? viewport.getWidth() - viewport.getScrollBarThickness()
        : viewport.getWidth();
    content.setOptimalSize(optimalWidth);
  }

  void setScrollbarThicknesses()
  {
    TRACER("SettingsEditor::setScrollbarThicknesses");
    const int scrollBarThickness =
      static_cast<int>(rawScrollBarThickness * size);
    categoryViewport.setScrollBarThickness(scrollBarThickness);
    editorViewport.setScrollBarThickness(scrollBarThickness);
  }

  void setScrollBarColour()
  {
    TRACER("SettingsEditor::setScrollBarColour");
    categoryViewport.getVerticalScrollBar().setColour(
      juce::ScrollBar::ColourIds::thumbColourId, scrollBarColour);
    editorViewport.getVerticalScrollBar().setColour(
      juce::ScrollBar::ColourIds::thumbColourId, scrollBarColour);

    categoryViewport.getVerticalScrollBar().setColour(
      juce::ScrollBar::ColourIds::backgroundColourId,
      scrollBarBackgroundColour);
    editorViewport.getVerticalScrollBar().setColour(
      juce::ScrollBar::ColourIds::backgroundColourId,
      scrollBarBackgroundColour);
  }

private:
  TreeAdapter treeAdapter;
  TextEditor searchEditor;
  Viewport categoryViewport;
  Viewport editorViewport;
  ValueEditorList valueEditorList;
  ValueCategoryList valueCategoryList;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsEditor)
};
} // namespace component
} // namespace gui
} // namespace dmt