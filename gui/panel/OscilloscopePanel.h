#pragma once
//==============================================================================
#include "gui/component/LinearSliderComponent.h"
#include "gui/display/OscilloscopeDisplay.h"
#include "gui/panel/AbstractPanel.h"
#include "utility/Unit.h"
#include <JuceHeader.h>
//==============================================================================
namespace dmt {
namespace gui {
namespace panel {
//==============================================================================
template<typename SampleType>
class OscilloscopePanel : public dmt::gui::panel::AbstractPanel
{
  using FifoAudioBuffer = dmt::dsp::data::FifoAudioBuffer<SampleType>;
  using OscilloscopeDisplay =
    dmt::gui::display::OscilloscopeDisplay<SampleType>;

  using LinearSlider = dmt::gui::component::LinearSliderComponent;
  using LinearSliderType = dmt::gui::widget::LinearSlider::Type;
  using LinearSliderOrientation = dmt::gui::widget::LinearSlider::Orientation;
  using Unit = dmt::utility::Unit;

  using Settings = dmt::Settings;

  const float& rawPadding = Settings::Panel::padding;

public:
  //============================================================================
  OscilloscopePanel(FifoAudioBuffer& fifoBuffer,
                    juce::AudioProcessorValueTreeState& apvts)
    : AbstractPanel("Oscilloscope", false)
    , oscilloscopeComponent(fifoBuffer, apvts)
    , zoomSlider(apvts,
                 juce::String("Zoom"),
                 juce::String("OscilloscopeZoom"),
                 Unit::Type::OscilloscopeZoom,
                 LinearSliderType::Positive,
                 LinearSliderOrientation::Vertical,
                 true)
    , thicknessSlider(apvts,
                      juce::String("Thickness"),
                      juce::String("OscilloscopeThickness"),
                      Unit::Type::OscilloscopeThickness,
                      LinearSliderType::Positive,
                      LinearSliderOrientation::Vertical,
                      true)
    , heightSlider(apvts,
                   juce::String("Gain"),
                   juce::String("OscilloscopeGain"),
                   Unit::Type::OscilloscopeHeight,
                   LinearSliderType::Bipolar,
                   LinearSliderOrientation::Vertical,
                   true)
  {
    addAndMakeVisible(oscilloscopeComponent);
    addAndMakeVisible(zoomSlider);
    addAndMakeVisible(thicknessSlider);
    addAndMakeVisible(heightSlider);
  }
  //============================================================================
  void extendResize() noexcept override
  {
    const auto padding = rawPadding * size;
    auto bounds = getLocalBounds().reduced(padding);

    const auto sliderWidth = 38 * size;

    auto leftSliderBounds = bounds.removeFromLeft(sliderWidth);
    const auto sliderOffsetX = 4.0f * size;
    leftSliderBounds =
      leftSliderBounds.withX(leftSliderBounds.getX() + sliderOffsetX);
    zoomSlider.setBounds(leftSliderBounds);

    auto rightSliderBounds = bounds.removeFromRight(sliderWidth);
    rightSliderBounds =
      rightSliderBounds.withX(rightSliderBounds.getX() - sliderOffsetX);

    thicknessSlider.setBounds(
      rightSliderBounds.removeFromTop(rightSliderBounds.getHeight() / 2));
    heightSlider.setBounds(rightSliderBounds);

    oscilloscopeComponent.setBounds(bounds);
  }
  //============================================================================
private:
  OscilloscopeDisplay oscilloscopeComponent;
  LinearSlider zoomSlider;
  LinearSlider thicknessSlider;
  LinearSlider heightSlider;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscilloscopePanel)
};

} // namespace panel
} // namespace gui
} // namespace dmt