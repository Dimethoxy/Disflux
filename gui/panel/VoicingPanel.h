//==============================================================================

#pragma once

#include "gui/component/LinearSliderComponent.h"
#include "gui/component/RotarySliderComponent.h"
#include "gui/panel/AbstractPanel.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
class VoicingPanel : public dmt::gui::panel::AbstractPanel
{

  using RotarySliderComponent = dmt::gui::component::RotarySliderComponent;
  using LinearSliderComponent = dmt::gui::component::LinearSliderComponent;
  using RotarySliderType = dmt::gui::widget::RotarySlider::Type;
  using LinearSliderType = dmt::gui::widget::LinearSlider::Type;
  using Unit = dmt::utility::Unit;

public:
  VoicingPanel(juce::AudioProcessorValueTreeState& apvts)
    : AbstractPanel("Voices")
    , osctaveSlider(apvts,
                    juce::String("Octave"),
                    juce::String("osc1VoiceOctave"),
                    Unit::Type::Octave,
                    RotarySliderType::Selector)
    , semitonesSlider(apvts,
                      juce::String("Semitone"),
                      juce::String("osc1VoiceSemitone"),
                      Unit::Type::Semitone,
                      RotarySliderType::Selector)
    , fineSlider(apvts,
                 juce::String("Fine"),
                 juce::String("osc1VoiceFine"),
                 Unit::Type::Cents,
                 LinearSliderType::Bipolar)
    , densitySlider(apvts,
                    juce::String("Density"),
                    juce::String("osc1VoiceDensity"),
                    Unit::Type::VoiceDensity,
                    RotarySliderType::Selector)
    , detuneSlider(apvts,
                   juce::String("Detune"),
                   juce::String("osc1VoiceDetune"),
                   Unit::Type::Cents)
    , distributionSlider(apvts,
                         juce::String("Distibution"),
                         juce::String("osc1VoiceDistribution"),
                         Unit::Type::VoiceDistribution)
    , widthSlider(apvts,
                  juce::String("Width"),
                  juce::String("osc1VoiceWidth"),
                  Unit::Type::Percent,
                  LinearSliderType::Positive)
    , blendSlider(apvts,
                  juce::String("Bend"),
                  juce::String("osc1VoiceBlend"),
                  Unit::Type::Percent,
                  LinearSliderType::Positive)
    , seedSlider(apvts,
                 juce::String("Seed"),
                 juce::String("osc1VoiceSeed"),
                 Unit::Type::Seed,
                 RotarySliderType::Selector)
    , randomlider(apvts,
                  juce::String("Random"),
                  juce::String("osc1VoiceRandom"),
                  Unit::Type::Percent)

    , phaseSlider(apvts,
                  juce::String("Phase"),
                  juce::String("osc1VoicePhase"),
                  Unit::Type::Degree,
                  LinearSliderType::Positive)
  {
    TRACER("VoicingPanel::VoicingPanel");
    setLayout({ 31, 32 });

    addAndMakeVisible(osctaveSlider);
    addAndMakeVisible(semitonesSlider);
    addAndMakeVisible(fineSlider);

    addAndMakeVisible(densitySlider);
    addAndMakeVisible(detuneSlider);
    addAndMakeVisible(distributionSlider);
    addAndMakeVisible(widthSlider);
    addAndMakeVisible(blendSlider);

    addAndMakeVisible(seedSlider);
    addAndMakeVisible(randomlider);
    addAndMakeVisible(phaseSlider);
  }
  void extendResize() noexcept override
  {
    TRACER("VoicingPanel::extendResize");
    const auto bounds = getLocalBounds();
    const int rotarySliderRow = 10;
    const int linearSliderRow = 26;

    //==============================================================================
    // Left Layout
    const int osctaveSliderCol = 3;
    const int semitoneSliderCol = 7;

    const auto osctaveSliderPoint =
      this->getGridPoint(bounds, osctaveSliderCol, rotarySliderRow);
    osctaveSlider.setSizeAndCentre(osctaveSliderPoint);

    const auto semitoneSliderPoint =
      this->getGridPoint(bounds, semitoneSliderCol, rotarySliderRow);
    semitonesSlider.setSizeAndCentre(semitoneSliderPoint);

    const int fineSliderPrimaryCol = 2;
    const int fineSliderSecondaryCol = 8;

    const auto fineSliderPrimaryPoint =
      this->getGridPoint(bounds, fineSliderPrimaryCol, linearSliderRow);
    const auto fineSliderSecondaryPoint =
      this->getGridPoint(bounds, fineSliderSecondaryCol, linearSliderRow);
    fineSlider.setBoundsByPoints(fineSliderPrimaryPoint,
                                 fineSliderSecondaryPoint);
    //==============================================================================
    // Middle Layout
    const int densitySliderCol = 12;
    const int detuneSliderCol = 16;
    const int distributionSliderCol = 20;

    const auto densitySliderPoint =
      this->getGridPoint(bounds, densitySliderCol, rotarySliderRow);
    densitySlider.setSizeAndCentre(densitySliderPoint);

    const auto detuneSliderPoint =
      this->getGridPoint(bounds, detuneSliderCol, rotarySliderRow);
    detuneSlider.setSizeAndCentre(detuneSliderPoint);

    const auto distributionSliderPoint =
      this->getGridPoint(bounds, distributionSliderCol, rotarySliderRow);
    distributionSlider.setSizeAndCentre(distributionSliderPoint);

    const int widthSliderPrimaryCol = 10;
    const int widthSliderSecondaryCol = 15;
    const int blendSliderPrimaryCol = 17;
    const int blendSliderSecondaryCol = 22;

    const auto blendSliderPrimaryPoint =
      this->getGridPoint(bounds, widthSliderPrimaryCol, linearSliderRow);
    const auto blendSliderSecondaryPoint =
      this->getGridPoint(bounds, widthSliderSecondaryCol, linearSliderRow);
    blendSlider.setBoundsByPoints(blendSliderPrimaryPoint,
                                  blendSliderSecondaryPoint);

    const auto widthSliderPrimaryPoint =
      this->getGridPoint(bounds, blendSliderPrimaryCol, linearSliderRow);
    const auto widthSliderSecondaryPoint =
      this->getGridPoint(bounds, blendSliderSecondaryCol, linearSliderRow);
    widthSlider.setBoundsByPoints(widthSliderPrimaryPoint,
                                  widthSliderSecondaryPoint);

    //==============================================================================
    // Right Layout
    const int seedSliderCol = 25;
    const int randomSliderCol = 29;

    const auto seedSliderPoint =
      this->getGridPoint(bounds, seedSliderCol, rotarySliderRow);
    seedSlider.setSizeAndCentre(seedSliderPoint);

    const auto randomSliderPoint =
      this->getGridPoint(bounds, randomSliderCol, rotarySliderRow);
    randomlider.setSizeAndCentre(randomSliderPoint);

    const int phaseSliderPrimaryCol = 24;
    const int phaseSliderSecondaryCol = 30;

    const auto phaseSliderPrimaryPoint =
      this->getGridPoint(bounds, phaseSliderPrimaryCol, linearSliderRow);
    const auto phaseSliderSecondaryPoint =
      this->getGridPoint(bounds, phaseSliderSecondaryCol, linearSliderRow);
    phaseSlider.setBoundsByPoints(phaseSliderPrimaryPoint,
                                  phaseSliderSecondaryPoint);
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoicingPanel)
  RotarySliderComponent osctaveSlider;
  RotarySliderComponent semitonesSlider;

  RotarySliderComponent densitySlider;
  RotarySliderComponent detuneSlider;
  RotarySliderComponent distributionSlider;

  RotarySliderComponent seedSlider;
  RotarySliderComponent randomlider;

  LinearSliderComponent fineSlider;
  LinearSliderComponent widthSlider;
  LinearSliderComponent blendSlider;
  LinearSliderComponent phaseSlider;
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt
