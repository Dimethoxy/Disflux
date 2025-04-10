#pragma once

#include "PluginProcessor.h"
#include <DmtHeader.h>

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
  using Image = juce::Image;
  using ImageComponent = juce::ImageComponent;
  using PixelFormat = juce::Image::PixelFormat;
  using OpenGLContext = juce::OpenGLContext;

  // Window size
  float& size = dmt::Settings::Window::size;
  const int baseWidth = 500;
  const int baseHeight = 270;

  // Window header
  const int& headerHeight = dmt::Settings::Header::height;

public:
  explicit PluginEditor(PluginProcessor&);
  ~PluginEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;
  void setConstraints(int width, int height);
  void handleHeaderVisibilityChange(bool isHeaderVisible);

private:
  //==============================================================================
  PluginProcessor& p;
  OpenGLContext openGLContext;
  //==============================================================================

  int lastWidth = baseWidth;
  int lastHeight = baseHeight;
  double ratio = baseWidth / baseHeight;
  //==============================================================================
  Image image;
  bool isResizing = false;
  //==============================================================================
  dmt::gui::panel::DisfluxPanel<float> disfluxPanel;
  dmt::gui::window::Compositor compositor;
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
