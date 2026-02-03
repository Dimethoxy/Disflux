//==============================================================================

#pragma once

#include "dsp/synth/AnalogOscillator.h"
#include "gui/widget/Shadow.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace component {
//==============================================================================
// TODO: Make this use the new display system
class OscillatorDisplayComponent
  : public juce::Component
  , public juce::Timer
  , public dmt::Scaleable<OscillatorDisplayComponent>
{
  using Shadow = dmt::gui::widget::Shadow;
  using AnalogOscillator = dmt::dsp::synth::AnalogOscillator;

  //============================================================================
  // General
  using Settings = dmt::Settings::OscillatorDisplay;
  const int& fps = dmt::Settings::framerate;
  const int& resolution = Settings::resolution;

  // Shadows
  const bool& drawOuterShadow = Settings::drawOuterShadow;
  const bool& drawInnerShadow = Settings::drawInnerShadow;
  const juce::Colour& outerShadowColour = Settings::outerShadowColour;
  const juce::Colour& innerShadowColour = Settings::innerShadowColour;
  const float& outerShadowRadius = Settings::outerShadowRadius;
  const float& innerShadowRadius = Settings::innerShadowRadius;

public:
  //============================================================================
  OscillatorDisplayComponent(juce::AudioProcessorValueTreeState& apvts)
    : apvts(apvts)
  {
    TRACER("OscillatorDisplayComponent::OscillatorDisplayComponent");
    osc.setSampleRate((float)resolution + 1.0f);
    osc.setFrequency(1.0f);
    startTimerHz(60);
  }
  //============================================================================
  void paint(juce::Graphics& g) override
  {
    TRACER("OscillatorDisplayComponent::paint");
    const auto bounds = this->getLocalBounds().toFloat();
  }

protected:
  //==============================================================================
  void timerCallback()
  {
    TRACER("OscillatorDisplayComponent::timerCallback");
    if (isParametersChanged()) {
      this->buildTable();
      this->repaint();
    }
  }

  void buildTable()
  {
    TRACER("OscillatorDisplayComponent::buildTable");
    osc.setPhase(0.0f);
    table.initialise(
      [&](std::size_t index) { return (float)osc.getNextSample(); },
      resolution);
  }

  bool isParametersChanged()
  {
    TRACER("OscillatorDisplayComponent::isParametersChanged");
  }

  //==============================================================================
  juce::Path getPath(juce::Rectangle<float> bounds)
  {
    TRACER("OscillatorDisplayComponent::getPath");
    bounds.setY(bounds.getY() + (bounds.getHeight() / 10.0f));
    bounds.setHeight(bounds.getHeight() - (bounds.getHeight() / 5.0f));

    auto outerBounds = bounds;
    bounds = bounds.reduced(bounds.getWidth() / 6.0f);

    juce::Path path;

    auto startX = bounds.getX();
    auto startY = bounds.getY() + (bounds.getHeight() / 2.0f);
    juce::Point<float> start(startX, startY);
    path.startNewSubPath(start);

    auto width = bounds.getWidth();

    for (size_t i = 0; i < width; i++) {
      auto x = bounds.getX() + i;
      auto y = bounds.getY() + (bounds.getHeight() / 2.0f) -
               (table[i / width * resolution] * bounds.getHeight() / 2.0f);
      juce::Point<float> p(x, y);
      path.lineTo(p);
    }

    auto endX = bounds.getX() + bounds.getWidth();
    auto endY = bounds.getY() + (bounds.getHeight() / 2.0f);
    juce::Point<float> end(endX, endY);
    path.lineTo(end);

    return path;
  }
  //============================================================================
private:
  Shadow outerShadow =
    Shadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false);
  Shadow innerShadow =
    Shadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true);

  AnalogOscillator osc;
  juce::dsp::LookupTable<float> table;
  juce::AudioProcessorValueTreeState& apvts;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorDisplayComponent)
};
} // namespace components
} // namespace gui
} // namespace dmt