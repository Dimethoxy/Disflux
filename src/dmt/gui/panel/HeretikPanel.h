//==============================================================================

#pragma once

//==============================================================================

#include "dsp/data/FifoAudioBuffer.h"
#include "gui/component/LinearSliderComponent.h"
#include "gui/component/RotarySliderComponent.h"
#include "gui/display/DisfluxDisplay.h"
#include "gui/panel/AbstractPanel.h"
#include "utility/Settings.h"
#include "utility/Unit.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace panel {

//==============================================================================
/**
 * @brief Panel for Disflux oscilloscope display and parameter controls.
 *
 * @tparam SampleType The floating-point sample type for audio data.
 *
 * @details
 * This class provides a GUI panel for the Disflux effect, including an
 * oscilloscope display and various parameter sliders. It inherits from
 * AbstractPanel and uses a grid layout for positioning components.
 */
template<typename SampleType>
class HeretikPanel : public dmt::gui::panel::AbstractPanel
{
  using FifoAudioBuffer = dmt::dsp::data::FifoAudioBuffer<float>;
  using DisfluxDisplay = dmt::gui::display::DisfluxDisplay;
  using LinearSlider = dmt::gui::component::LinearSliderComponent;
  using LinearSliderType = dmt::gui::widget::LinearSlider::Type;
  using LinearSliderOrientation = dmt::gui::widget::LinearSlider::Orientation;
  using RotarySlider = dmt::gui::component::RotarySliderComponent;
  using RotarySliderType = dmt::gui::widget::RotarySlider::Type;
  using Unit = dmt::utility::Unit;
  using Settings = dmt::Settings;
  const float& rawPadding = Settings::Panel::padding;

public:
  //==============================================================================
  /**
   * @brief Constructs a DisfluxPanel.
   *
   * @param _apvts The AudioProcessorValueTreeState for parameter binding.
   * @param _oscilloscopeBuffer The FIFO buffer for oscilloscope display.
   *
   * @details
   * Initializes all display and slider components, sets up the grid layout,
   * and adds all subcomponents to the panel.
   */
  constexpr inline explicit HeretikPanel(
    juce::AudioProcessorValueTreeState& _apvts,
    FifoAudioBuffer& _oscilloscopeBuffer) noexcept
    : AbstractPanel("Oscilloscope", false)
    , display(_oscilloscopeBuffer, _apvts)
    , driveSlider(_apvts,
                  juce::String("PreGain"),
                  juce::String("HeretikPreGain"),
                  Unit::Type::HeretikPreGain,
                  RotarySliderType::Bipolar)
    , rangeSlider(_apvts,
                  juce::String("Stereo"),
                  juce::String("HeretikSpread"),
                  Unit::Type::HeretikStereo,
                  RotarySliderType::Positive)
    , toneSlider(_apvts,
                 juce::String("Range"),
                 juce::String("HeretikRange"),
                 Unit::Type::HeretikRange,
                 LinearSliderType::Positive,
                 LinearSliderOrientation::Horizontal)
    , feedbackSlider(_apvts,
                     juce::String("Saturation"),
                     juce::String("HeretikDistortion"),
                     Unit::Type::HeretikDistortion,
                     RotarySliderType::Positive)
    , mixSlider(_apvts,
                juce::String("Mix"),
                juce::String("HeretikMix"),
                Unit::Type::HeretikMix,
                RotarySliderType::Positive)
  {
    TRACER("DisfluxPanel::DisfluxPanel");
    setLayout({ 60, 60 });

    addAndMakeVisible(display);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(rangeSlider);
    addAndMakeVisible(toneSlider);
    addAndMakeVisible(feedbackSlider);
    addAndMakeVisible(mixSlider);
  }

  //==============================================================================
  /**
   * @brief Handles resizing and layout of all subcomponents.
   *
   * @details
   * Positions the oscilloscope display and all sliders using grid points,
   * ensuring consistent layout regardless of panel size.
   * Override from AbstractPanel.
   */
  inline void extendResize() noexcept override
  {
    TRACER("DisfluxPanel::extendResize");
    auto bounds = getLocalBounds();

    const float padding = rawPadding * size;
    auto displayBounds = bounds.reduced(padding);
    const float displayHorizontalPadding = 90.0f;
    const float displayVerticalPadding = 53.0f;
    displayBounds.removeFromBottom(displayVerticalPadding * size);
    displayBounds.removeFromLeft(displayHorizontalPadding * size);
    displayBounds.removeFromRight(displayHorizontalPadding * size);
    display.setBounds(displayBounds);

    const int upperRotarySliderRow = 16;
    const int lowerRotarySliderRow = 43;
    const int linearSliderRow = 51;

    const int driveSliderCol = 8;
    const int leftToneSliderCol = 16;
    const int rightToneSliderCol = 44;
    const int feedbackSliderCol = 53;

    const auto driveSliderPoint =
      this->getGridPoint(bounds, driveSliderCol, upperRotarySliderRow);
    driveSlider.setSizeAndCentre(driveSliderPoint);

    const auto rangeSliderPoint =
      this->getGridPoint(bounds, driveSliderCol, lowerRotarySliderRow);
    rangeSlider.setSizeAndCentre(rangeSliderPoint);

    const auto leftToneSliderPoint =
      this->getGridPoint(bounds, leftToneSliderCol, linearSliderRow);
    const auto rightToneSliderPoint =
      this->getGridPoint(bounds, rightToneSliderCol, linearSliderRow);
    toneSlider.setBoundsByPoints(leftToneSliderPoint, rightToneSliderPoint);

    const auto feedbackSliderPoint =
      this->getGridPoint(bounds, feedbackSliderCol, lowerRotarySliderRow);
    feedbackSlider.setSizeAndCentre(feedbackSliderPoint);

    const auto mixSliderPoint =
      this->getGridPoint(bounds, feedbackSliderCol, upperRotarySliderRow);
    mixSlider.setSizeAndCentre(mixSliderPoint);
  }
  //==============================================================================

private:
  //==============================================================================
  // Members initialized in the initializer list
  DisfluxDisplay display;
  RotarySlider driveSlider;
  RotarySlider rangeSlider;
  LinearSlider toneSlider;
  RotarySlider feedbackSlider;
  RotarySlider mixSlider;

  //==============================================================================
  // Other members

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeretikPanel)
};

} // namespace panel
} // namespace gui
} // namespace dmt