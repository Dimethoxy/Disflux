//==============================================================================

#pragma once

//==============================================================================

#include "dmt/gui/widget/ValueEditor.h"
#include "dmt/utility/Fonts.h"
#include "dmt/utility/Scaleable.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace component {
//==============================================================================
class ValueCategoryList
  : public juce::Component
  , public dmt::Scaleable<ValueCategoryList>
{
  using Colour = juce::Colour;
  using Settings = dmt::Settings;
  using SettingsEditorSettings = dmt::Settings::SettingsEditor;
  using String = juce::String;
  using Fonts = dmt::utility::Fonts;
  using Lable = dmt::gui::widget::Label;
  using LabelPointer = std::unique_ptr<Lable>;
  using LabelList = std::vector<LabelPointer>;
  using TreeAdapter = dmt::configuration::TreeAdapter;
  using CategoryList = std::vector<TreeAdapter::Category>;
  using CategoryCallback = std::function<void(TreeAdapter::Category&)>;

  //==============================================================================
  // TODO: Move to settings
  const Colour& fontColour = SettingsEditorSettings::fontColour;
  const Colour& selectedFontColour = SettingsEditorSettings::selectedFontColour;
  const Colour& selectedBackgroundColour =
    SettingsEditorSettings::selectedLabelBackgroundColour;
  const Colour& seperatorColour = SettingsEditorSettings::seperatorColour;
  const float& rawFontSize = SettingsEditorSettings::fontSize;
  const float& labelHorizontalPadding =
    SettingsEditorSettings::labelHorizontalPadding;

public:
  ValueCategoryList(CategoryList& _categories,
                    CategoryCallback _onCategorySelected)
    : categories(_categories)
    , onCategorySelected(std::move(_onCategorySelected))
  {
    TRACER("ValueCategoryList::ValueCategoryList");
    addCategories();
    onCategorySelected(categories[0]);
    labelList[0]->setFontColour(selectedFontColour);
    labelList[0]->setBackgroundColour(selectedBackgroundColour);
  }

  ~ValueCategoryList() override = default;

  void paint(juce::Graphics& _g) override
  {
    TRACER("ValueCategoryList::paint");
    _g.setColour(seperatorColour);

    // Draw top line
    _g.drawLine(0.0f, 0.0f, static_cast<float>(getWidth()), 0.0f, 1.0f);

    // Draw separator lines at the top of each child component (except the
    // first)
    for (int i = 1; i < getNumChildComponents(); ++i) {
      auto* comp = getChildComponent(i);
      if (comp) {
        float y = static_cast<float>(comp->getY());
        _g.drawLine(0.0f, y, static_cast<float>(getWidth()), y, 1.0f);
      }
    }

    // Draw bottom line at the bottom of the last child component
    if (getNumChildComponents() > 0) {
      auto* last = getChildComponent(getNumChildComponents() - 1);
      if (last) {
        float bottomY = static_cast<float>(last->getBottom());
        _g.drawLine(
          0.0f, bottomY, static_cast<float>(getWidth()), bottomY, 1.0f);
      }
    }
  }

  void resized() override
  {
    TRACER("ValueCategoryList::resized");
    const auto fontSize = rawFontSize * size;
    auto bounds = getLocalBounds();
    for (auto& label : labelList) {
      auto labelBounds = bounds.removeFromTop(fontSize);
      label->setBounds(labelBounds);
    }
  }

  void setOptimalSize(const int width)
  {
    TRACER("ValueCategoryList::setOptimalSize");
    const auto fontSize = rawFontSize * size;
    const auto neededHeight = fontSize * labelList.size();
    const auto extraHeight = fontSize * 0.5f;
    setSize(width, neededHeight + extraHeight);
  }

protected:
  void addCategories()
  {
    TRACER("ValueCategoryList::addCategories");
    // Let's clear the current labels
    labelList.clear();

    // And generate new ones
    for (auto& category : categories) {
      const auto text = String(category.name);
      auto label = std::make_unique<Lable>(text,
                                           fonts.medium,
                                           rawFontSize,
                                           fontColour,
                                           juce::Justification::centredLeft);
      label->setRawHorizontalPadding(5.0f);
      labelList.push_back(std::move(label));
    }
    addAllLabels();
  }

  void addAllLabels()
  {
    TRACER("ValueCategoryList::addAllLabels");
    for (auto& label : labelList) {
      addAndMakeVisible(*label);
      label->addMouseListener(this, true);
    }
  }

  void mouseUp(const juce::MouseEvent& _event) override
  {
    TRACER("ValueCategoryList::mouseUp");
    const auto clickedLabel = dynamic_cast<Lable*>(_event.eventComponent);

    if (!clickedLabel)
      return;

    for (auto& label : labelList) {
      if (label.get() != clickedLabel) {
        label->setFontColour(fontColour);
        label->setBackgroundColour(juce::Colours::transparentBlack);
      }
    }
    clickedLabel->setFontColour(selectedFontColour);
    clickedLabel->setBackgroundColour(selectedBackgroundColour);

    for (std::size_t i = 0; i < labelList.size(); ++i) {
      if (labelList[i].get() == clickedLabel) {
        onCategorySelected(categories[i]);
        break;
      }
    }
  }

private:
  //==============================================================================
  CategoryList& categories;

  //==============================================================================
  // Other members
  CategoryCallback onCategorySelected;
  LabelList labelList;
  Fonts fonts;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValueCategoryList)
};

} // namespace component
} // namespace gui
} // namespace dmt