#pragma once

#include "PluginProcessor.h"
#include <DmtHeader.h>

//==============================================================================
class PluginEditor final
  : public juce::AudioProcessorEditor
  , juce::Timer
{
  using Image = juce::Image;
  using PixelFormat = juce::Image::PixelFormat;
  using OpenGLContext = juce::OpenGLContext;

  //==============================================================================
  float& size = dmt::Settings::Window::size;

public:
  explicit PluginEditor(PluginProcessor&);
  ~PluginEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;
  void parentSizeChanged() override;
  void timerCallback() override;

private:
  //==============================================================================
  PluginProcessor& p;
  OpenGLContext openGLContext;
  //==============================================================================
  int baseWidth = 800;
  int baseHeight = 400;
  //==============================================================================
  Image image;
  bool isResizing = false;
  //==============================================================================
  dmt::gui::panel::OscilloscopePanel<float> oscilloscopePanel;
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
