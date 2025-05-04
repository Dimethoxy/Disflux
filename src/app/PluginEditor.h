#pragma once

#include "PluginProcessor.h"
#include <DmtHeader.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
class PluginEditor
  : public juce::AudioProcessorEditor
  , private juce::Timer
{
  using Image = juce::Image;
  using ImageComponent = juce::ImageComponent;
  using PixelFormat = juce::Image::PixelFormat;
  using OpenGLContext = juce::OpenGLContext;

  // Window size
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

  // Debounced resizing
  void timerCallback() override;
  void detachCompositorForResize();
  void attachCompositorAfterResize();
  void updateCompositorSnapshot();

private:
  //==============================================================================
  PluginProcessor& p;
  OpenGLContext openGLContext;
  //==============================================================================

  int lastWidth = baseWidth;
  int lastHeight = baseHeight;
  double ratio = baseWidth / baseHeight;
  float sizeFactor = 1.0f;
  //==============================================================================
  Image image;
  bool isResizing = false;
  //==============================================================================
  dmt::gui::panel::DisfluxPanel<float> disfluxPanel;
  dmt::gui::window::Compositor compositor;
  //==============================================================================
  juce::Image compositorSnapshot;
  bool compositorAttached = true;
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
