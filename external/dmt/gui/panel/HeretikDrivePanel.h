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
class HeretikDrivePanel : public dmt::gui::panel::AbstractPanel
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
  constexpr inline explicit HeretikDrivePanel(
    juce::AudioProcessorValueTreeState& _apvts,
    FifoAudioBuffer& _oscilloscopeBuffer) noexcept
    : AbstractPanel("Oscilloscope", false)
    , driveTypeSlider(_apvts,
                      juce::String("Type"),
                      juce::String("HeretikDriveType"),
                      Unit::Type::HeretikDriveType,
                      RotarySliderType::Selector)
    , driveSlider(_apvts,
                  juce::String("Amount"),
                  juce::String("HeretikDrive"),
                  Unit::Type::HeretikDrive,
                  LinearSliderType::Positive,
                  LinearSliderOrientation::Horizontal)
    , biasSlider(_apvts,
                 juce::String("Symmetry"),
                 juce::String("HeretikDriveBias"),
                 Unit::Type::HeretikDriveBias,
                 RotarySliderType::Bipolar)
  {
    TRACER("DisfluxPanel::DisfluxPanel");
    setLayout({ 20, 60 });

    addAndMakeVisible(driveTypeSlider);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(biasSlider);
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

    const int upperRotarySliderRow = 21;
    const int linearSliderRow = 49;

    const int driveSliderCol = 6;
    const int feedbackSliderCol = 15;

    const int leftToneSliderCol = 3;
    const int rightToneSliderCol = 18;

    const auto driveSliderPoint =
      this->getGridPoint(bounds, driveSliderCol, upperRotarySliderRow);
    driveTypeSlider.setSizeAndCentre(driveSliderPoint);

    const auto mixSliderPoint =
      this->getGridPoint(bounds, feedbackSliderCol, upperRotarySliderRow);
    biasSlider.setSizeAndCentre(mixSliderPoint);

    const auto leftToneSliderPoint =
      this->getGridPoint(bounds, leftToneSliderCol, linearSliderRow);
    const auto rightToneSliderPoint =
      this->getGridPoint(bounds, rightToneSliderCol, linearSliderRow);
    driveSlider.setBoundsByPoints(leftToneSliderPoint, rightToneSliderPoint);
  }
  //==============================================================================

private:
  //==============================================================================
  // Members initialized in the initializer list
  RotarySlider driveTypeSlider;
  LinearSlider driveSlider;
  RotarySlider biasSlider;

  //==============================================================================
  // Other members

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeretikDrivePanel)
};

} // namespace panel
} // namespace gui
} // namespace dmt