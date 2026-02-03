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
class WaveformDistortionPanel : public dmt::gui::panel::AbstractPanel
{
  using RotarySliderComponent = dmt::gui::component::RotarySliderComponent;
  using LinearSliderComponent = dmt::gui::component::LinearSliderComponent;
  using RotarySliderType = dmt::gui::widget::RotarySlider::Type;
  using LinearSliderType = dmt::gui::widget::LinearSlider::Type;
  using Unit = dmt::utility::Unit;

public:
  WaveformDistortionPanel(juce::AudioProcessorValueTreeState& apvts)
    : AbstractPanel("Waveform Distortion")
    , typeSlider(apvts,
                 juce::String("Type"),
                 juce::String("osc1DistortionType"),
                 Unit::Type::DistortionType,
                 RotarySliderType::Selector)
    , gainSlider(apvts,
                 juce::String("Pregain"),
                 juce::String("osc1DistortionPreGain"),
                 Unit::Type::Gain)
    , driveSlider(apvts,
                  juce::String("Drive"),
                  juce::String("osc1DistortionDrive"),
                  Unit::Type::Drive)
    , biasSlider(apvts,
                 juce::String("Symmetry"),
                 juce::String("osc1DistortionSymmetry"),
                 Unit::Type::Symmetry,
                 LinearSliderType::Bipolar)
    , crushSlider(apvts,
                  juce::String("Bitcrush"),
                  juce::String("osc1DistortionCrush"),
                  Unit::Type::Bitdepth)
  {
    TRACER("WaveformDistortionPanel::WaveformDistortionPanel");
    setLayout({ 17, 32 });
    addAndMakeVisible(typeSlider);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(biasSlider);
    addAndMakeVisible(crushSlider);
  }
  void extendResize() noexcept override
  {
    TRACER("WaveformDistortionPanel::extendResize");
    const auto bounds = getLocalBounds();
    const int rotarySliderRow = 10;
    const int linearSliderRow = 26;

    const int typeSliderCol = 4;
    const int gainSliderCol = 9;
    const int driveSliderCol = 14;

    const int biasPrimaryCol = 2;
    const int biasSecondaryCol = 8;
    const int crushPrimaryCol = 10;
    const int crushSecondaryCol = 16;

    const auto typeSliderPoint =
      this->getGridPoint(bounds, typeSliderCol, rotarySliderRow);
    typeSlider.setSizeAndCentre(typeSliderPoint);

    const auto gainSliderPoint =
      this->getGridPoint(bounds, gainSliderCol, rotarySliderRow);
    gainSlider.setSizeAndCentre(gainSliderPoint);

    const auto driveSliderPoint =
      this->getGridPoint(bounds, driveSliderCol, rotarySliderRow);
    driveSlider.setSizeAndCentre(driveSliderPoint);

    const auto biasSliderPrimaryPoint =
      this->getGridPoint(bounds, biasPrimaryCol, linearSliderRow);
    const auto biasSliderSecondaryPoint =
      this->getGridPoint(bounds, biasSecondaryCol, linearSliderRow);
    biasSlider.setBoundsByPoints(biasSliderPrimaryPoint,
                                 biasSliderSecondaryPoint);

    const auto crushSliderPrimaryPoint =
      this->getGridPoint(bounds, crushPrimaryCol, linearSliderRow);
    const auto crushSliderSecondaryPoint =
      this->getGridPoint(bounds, crushSecondaryCol, linearSliderRow);
    crushSlider.setBoundsByPoints(crushSliderPrimaryPoint,
                                  crushSliderSecondaryPoint);
  }

  RotarySliderComponent typeSlider;
  RotarySliderComponent gainSlider;
  RotarySliderComponent driveSlider;
  LinearSliderComponent biasSlider;
  LinearSliderComponent crushSlider;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDistortionPanel)
};
//==============================================================================
} // namespace panel
} // namespace gui
} // namespace dmt
//==============================================================================