//==============================================================================
#pragma once
//==============================================================================
#include "gui/component/LinearSliderComponent.h"
#include "gui/component/RotarySliderComponent.h"
#include "gui/panel/AbstractPanel.h"
#include "utility/Unit.h"
#include <JuceHeader.h>
//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class OscSendPanel : public dmt::gui::panel::AbstractPanel
{
  using RotarySliderComponent = dmt::gui::component::RotarySliderComponent;
  using LinearSliderComponent = dmt::gui::component::LinearSliderComponent;
  using RotarySliderType = dmt::gui::widget::RotarySlider::Type;
  using LinearSliderType = dmt::gui::widget::LinearSlider::Type;
  using Unit = dmt::utility::Unit;

public:
  OscSendPanel(juce::AudioProcessorValueTreeState& apvts,
               const juce::String channel)
    : AbstractPanel(juce::String("Channel " + channel))
    , channel(channel)
    , gainSlider(apvts,
                 juce::String("Gain"),
                 juce::String("osc1Send" + channel + "Gain"),
                 Unit::Type::Gain)
    , panSlider(apvts,
                juce::String("Pan"),
                juce::String("osc1Send" + channel + "Pan"),
                Unit::Type::Pan,
                LinearSliderType::Bipolar)
  {
    TRACER("OscSendPanel::OscSendPanel");
    setLayout({ 3, 32 });
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(panSlider);
  }

  void extendResize() noexcept override
  {
    TRACER("OscSendPanel::extendResize");
    auto bounds = getLocalBounds();
    auto gainSliderPoint = this->getGridPoint(bounds, 2, 10);
    gainSlider.setSizeAndCentre(gainSliderPoint);
    auto panSliderPrimaryPoint = this->getGridPoint(bounds, 1, 26);
    auto panSliderSecundaryPoint = this->getGridPoint(bounds, 3, 26);
    panSlider.setBoundsByPoints(panSliderPrimaryPoint, panSliderSecundaryPoint);
  }

private:
  const juce::String channel;
  RotarySliderComponent gainSlider;
  LinearSliderComponent panSlider;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscSendPanel)
};
//==============================================================================
} // namespace panel
} // namespace gui
} // namespace dmt
//==============================================================================
