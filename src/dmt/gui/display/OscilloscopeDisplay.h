#pragma once
//==============================================================================
#include "dsp/data/FifoAudioBuffer.h"
#include "dsp/data/RingAudioBuffer.h"
#include "gui/display/AbstractDisplay.h"
#include "gui/widget/Oscilloscope.h"
#include "gui/widget/Shadow.h"
#include "utility/RepaintTimer.h"
#include "utility/Settings.h"
#include <JuceHeader.h>
//==============================================================================
namespace dmt {
namespace gui {
namespace display {
//==============================================================================
template<typename SampleType>
class OscilloscopeDisplay
  : public dmt::gui::display::AbstractDisplay
  , public juce::AudioProcessorValueTreeState::Listener
{
  using Oscilloscope = dmt::gui::widget::Oscilloscope<SampleType>;
  using RingAudioBuffer = dmt::dsp::data::RingAudioBuffer<SampleType>;
  using FifoAudioBuffer = dmt::dsp::data::FifoAudioBuffer<SampleType>;
  using Shadow = dmt::gui::widget::Shadow;
  using Colour = juce::Colour;
  using Settings = dmt::Settings;
  using DisplaySettings = dmt::Settings::Display;

  //==============================================================================
  // General
  const Colour& backgroundColour = DisplaySettings::backgroundColour;

  // Layout
  const float& rawCornerSize = DisplaySettings::cornerSize;
  const float& rawPadding = DisplaySettings::padding;
  // Border
  const bool& drawBorder = DisplaySettings::drawBorder;
  const Colour& borderColour = DisplaySettings::borderColour;
  const float& borderStrength = DisplaySettings::borderStrength;
  // Shadows
  const bool& drawOuterShadow = DisplaySettings::drawOuterShadow;
  const bool& drawInnerShadow = DisplaySettings::drawInnerShadow;
  const Colour& outerShadowColour = DisplaySettings::outerShadowColour;
  const Colour& innerShadowColour = DisplaySettings::innerShadowColour;
  const float& outerShadowRadius = DisplaySettings::outerShadowRadius;
  const float& innerShadowRadius = DisplaySettings::innerShadowRadius;

public:
  //==============================================================================
  OscilloscopeDisplay(FifoAudioBuffer& _fifoBuffer,
                      AudioProcessorValueTreeState& _apvts,
                      bool _useDefaultSettings = false)
    : ringBuffer(2, 4096)
    , fifoBuffer(_fifoBuffer)
    , leftOscilloscope(ringBuffer, 0, size)
    , rightOscilloscope(ringBuffer, 1, size)
    , useDefaultSettings(_useDefaultSettings)
  {
    if (!useDefaultSettings) {
      _apvts.addParameterListener("OscilloscopeZoom", this);
      _apvts.addParameterListener("OscilloscopeThickness", this);
      _apvts.addParameterListener("OscilloscopeGain", this);
    } else {
      // Use default values from dmt::Settings::Oscilloscope
      setZoom(dmt::Settings::Oscilloscope::defaultZoom);
      setThickness(dmt::Settings::Oscilloscope::defaultThickness);
      setHeight(dmt::Settings::Oscilloscope::defaultGain);
    }
  }
  //==============================================================================
  void extendResized(
    const juce::Rectangle<int>& _displayBounds) noexcept override
  {
    auto scopeBounds =
      _displayBounds.withHeight(_displayBounds.getHeight() * 0.92f)
        .withCentre(_displayBounds.getCentre());

    auto leftScopeBounds =
      scopeBounds.removeFromTop(scopeBounds.getHeight() * 0.5f);
    auto rightScopeBounds = scopeBounds;

    leftOscilloscope.setBounds(leftScopeBounds);
    rightOscilloscope.setBounds(rightScopeBounds);

    // If using default settings, update oscilloscope parameters on resize
    if (useDefaultSettings) {
      setZoom(dmt::Settings::Oscilloscope::defaultZoom);
      setThickness(dmt::Settings::Oscilloscope::defaultThickness);
      setHeight(dmt::Settings::Oscilloscope::defaultGain);
    }
  }
  //==============================================================================
  void paintDisplay(
    juce::Graphics& g,
    const juce::Rectangle<int>& /*_displayBounds*/) noexcept override
  {
    TRACER("OscilloscopeDisplay::paintDisplay");

    const auto leftScopeBounds = leftOscilloscope.getBounds().toFloat();
    const auto rightScopeBounds = rightOscilloscope.getBounds().toFloat();

    g.setColour(backgroundColour.brighter(0.05));

    // Draw vertical lines for both scopes
    drawVerticalLines(g,
                      leftScopeBounds.getX(),
                      leftScopeBounds.getWidth(),
                      leftScopeBounds.getY(),
                      leftScopeBounds.getHeight());
    drawVerticalLines(g,
                      leftScopeBounds.getX(),
                      leftScopeBounds.getWidth(),
                      rightScopeBounds.getY(),
                      rightScopeBounds.getHeight());

    // Draw horizontal lines for both scopes
    drawHorizontalLines(g,
                        leftScopeBounds.getX(),
                        leftScopeBounds.getWidth(),
                        leftScopeBounds.getY(),
                        leftScopeBounds.getHeight());
    drawHorizontalLines(g,
                        leftScopeBounds.getX(),
                        leftScopeBounds.getWidth(),
                        rightScopeBounds.getY(),
                        rightScopeBounds.getHeight());

    // Draw oscilloscope images
    g.drawImageAt(leftOscilloscope.getImage(),
                  leftOscilloscope.getBounds().getX(),
                  leftOscilloscope.getBounds().getY());
    g.drawImageAt(rightOscilloscope.getImage(),
                  rightOscilloscope.getBounds().getX(),
                  rightOscilloscope.getBounds().getY());
  }

protected:
  //==============================================================================
  void drawVerticalLines(juce::Graphics& g,
                         float scopeX,
                         float scopeWidth,
                         float scopeY,
                         float scopeHeight) const
  {
    const int numLines = getWidth() / (getHeight() / 4.0f);
    const float lineSpacing = scopeWidth / numLines;

    for (size_t i = 1; i < numLines; ++i) {
      const float x = scopeX + lineSpacing * i;
      g.drawLine(juce::Line<float>(x, scopeY, x, scopeY + scopeHeight),
                 2.0f * size);
    }
  }
  //==============================================================================
  void drawHorizontalLines(juce::Graphics& g,
                           float scopeX,
                           float scopeWidth,
                           float scopeY,
                           float scopeHeight) const
  {
    float lineThicknessModifiers[7] = {
      1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f
    };
    float brightnessValues[7] = { 0.15f, 0.05f, 0.05f, 0.05f,
                                  0.05f, 0.05f, 0.15f };

    for (size_t i = 0; i < 7; ++i) {
      const float y = scopeY + (scopeHeight / 6) * i;
      g.setColour(backgroundColour.brighter(brightnessValues[i]));
      g.drawLine(juce::Line<float>(scopeX, y, scopeX + scopeWidth, y),
                 3.0f * lineThicknessModifiers[i] * size);
    }
  }
  //==============================================================================
  void prepareNextFrame() noexcept override
  {
    TRACER("OscilloscopeDisplay::prepareNextFrame");
    ringBuffer.write(fifoBuffer);
    ringBuffer.equalizeReadPositions();
    leftOscilloscope.notify();
    rightOscilloscope.notify();
  }
  //==============================================================================
  void setZoom(float _zoom) noexcept
  {
    TRACER("OscilloscopeDisplay::setZoom");
    // Just random math with magic numbers to get a nice feeling zoom
    float zoomModifier = (_zoom + 5) / 105.0f;
    float maxSamplesPerPixel = 900.0f;
    float exponentialModifier = pow(zoomModifier, 4.0f);
    float samplesPerPixel = 1.0f + maxSamplesPerPixel * exponentialModifier;
    leftOscilloscope.setRawSamplesPerPixel(samplesPerPixel);
    rightOscilloscope.setRawSamplesPerPixel(samplesPerPixel);
  }
  //==============================================================================
  void setThickness(float _thickness) noexcept
  {
    TRACER("OscilloscopeDisplay::setThickness");
    leftOscilloscope.setThickness(_thickness);
    rightOscilloscope.setThickness(_thickness);
  }
  //==============================================================================
  void setHeight(float _height) noexcept
  {
    TRACER("OscilloscopeDisplay::setHeight");
    float amplitude = juce::Decibels::decibelsToGain(_height);
    leftOscilloscope.setAmplitude(amplitude);
    rightOscilloscope.setAmplitude(amplitude);
  }
  //==============================================================================
  virtual void parameterChanged(const String& _parameterID, float _newValue)
  {
    if (_parameterID == "OscilloscopeZoom") {
      setZoom(_newValue);
    }
    if (_parameterID == "OscilloscopeThickness") {
      setThickness(_newValue);
    }
    if (_parameterID == "OscilloscopeGain") {
      setHeight(_newValue);
    }
  }
  //==============================================================================
private:
  RingAudioBuffer ringBuffer;
  FifoAudioBuffer& fifoBuffer;
  Oscilloscope leftOscilloscope;
  Oscilloscope rightOscilloscope;
  bool useDefaultSettings;
  //==============================================================================

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscilloscopeDisplay)
};
} // namespace component
} // namespace gui
} // namespace dmt