//==============================================================================

#pragma once

#include "gui/component/LinearSliderComponent.h"
#include "gui/panel/AbstractPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class AnalogGainPanel : public dmt::gui::panel::AbstractPanel
{
  using Unit = dmt::utility::Unit;

  using LinearSliderComponent = dmt::gui::component::LinearSliderComponent;
  using LinearSliderType = dmt::gui::widget::LinearSlider::Type;
  using LinearSliderOrientation = dmt::gui::widget::LinearSlider::Orientation;

public:
  AnalogGainPanel(juce::AudioProcessorValueTreeState& apvts)
    : AbstractPanel("Analog Gain Envelope")
    , attackSlider(apvts,
                   juce::String("Attack"),
                   juce::String("osc1GainEnvAttack"),
                   Unit::Type::Milliseconds,
                   LinearSliderType::Positive,
                   LinearSliderOrientation::Vertical)
    , holdSlider(apvts,
                 juce::String("Hold"),
                 juce::String("osc1GainEnvHold"),
                 Unit::Type::Milliseconds,
                 LinearSliderType::Positive,
                 LinearSliderOrientation::Vertical)
    , decaySlider(apvts,
                  juce::String("Decay"),
                  juce::String("osc1GainEnvDecay"),
                  Unit::Type::Milliseconds,
                  LinearSliderType::Positive,
                  LinearSliderOrientation::Vertical)
    , skewSlider(apvts,
                 juce::String("Skew"),
                 juce::String("osc1GainEnvSkew"),
                 Unit::Type::EnvelopeSkew,
                 LinearSliderType::Positive,
                 LinearSliderOrientation::Vertical)
  {
    TRACER("AnalogGainPanel::AnalogGainPanel");
    setLayout({ 25, 32 });
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(holdSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(skewSlider);
  }

  void extendResize() noexcept override
  {
    TRACER("AnalogGainPanel::extendResize");
    const auto bounds = getLocalBounds();

    const int primaryRow = 3;
    const int secundaryRow = 28;

    const int attackCol = 7;
    const int holdCol = 11;
    const int decayCol = 15;
    const int skewCol = 19;

    const auto attackSliderPrimaryPoint =
      this->getGridPoint(bounds, attackCol, primaryRow);
    const auto attackSliderSecondaryPoint =
      this->getGridPoint(bounds, attackCol, secundaryRow);
    attackSlider.setBoundsByPoints(attackSliderPrimaryPoint,
                                   attackSliderSecondaryPoint);

    const auto holdSliderPrimaryPoint =
      this->getGridPoint(bounds, holdCol, primaryRow);
    const auto holdSliderSecondaryPoint =
      this->getGridPoint(bounds, holdCol, secundaryRow);
    holdSlider.setBoundsByPoints(holdSliderPrimaryPoint,
                                 holdSliderSecondaryPoint);

    const auto decaySliderPrimaryPoint =
      this->getGridPoint(bounds, decayCol, primaryRow);
    const auto decaySliderSecondaryPoint =
      this->getGridPoint(bounds, decayCol, secundaryRow);
    decaySlider.setBoundsByPoints(decaySliderPrimaryPoint,
                                  decaySliderSecondaryPoint);

    const auto skewSliderPrimaryPoint =
      this->getGridPoint(bounds, skewCol, primaryRow);
    const auto skewSliderSecondaryPoint =
      this->getGridPoint(bounds, skewCol, secundaryRow);
    skewSlider.setBoundsByPoints(skewSliderPrimaryPoint,
                                 skewSliderSecondaryPoint);
  }

private:
  LinearSliderComponent attackSlider;
  LinearSliderComponent holdSlider;
  LinearSliderComponent decaySlider;
  LinearSliderComponent skewSlider;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogGainPanel)
};
//==============================================================================
} // namespace panel
} // namespace gui
} // namespace dmt
