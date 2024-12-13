#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
PluginEditor::PluginEditor(PluginProcessor& p)
  : AudioProcessorEditor(&p)
  , p(p)
  , disfluxPanel(p.apvts)
{
  if (OS_IS_WINDOWS) {
    setResizable(true, true);
  }

  if (OS_IS_DARWIN) {
    setResizable(true, true);
  }

  if (OS_IS_LINUX) {
    openGLContext.setComponentPaintingEnabled(true);
    openGLContext.setContinuousRepainting(false);
    openGLContext.attachTo(*getTopLevelComponent());
    setResizable(false, true);
  }

  auto& lnf = this->getLookAndFeel();
  lnf.setUsingNativeAlertWindows(true);
  lnf.setColour(juce::ResizableWindow::backgroundColourId,
                dmt::Settings::Colours::background);

  addAndMakeVisible(disfluxPanel);

  // dmt::Settings::debugBounds = true;
  // dmt::Settings::debugGrid = true;

  setResizable(true, true);
  setResizeLimits(baseWidth, baseWidth / ratio, 4000, 4000 / ratio);
  // getConstrainer()->setFixedAspectRatio(ratio);
  setSize(baseWidth, baseWidth / ratio);
}
//==============================================================================
PluginEditor::~PluginEditor() {}

//==============================================================================
void
PluginEditor::timerCallback()
{
  TRACER("PluginEditor::timerCallback");

  // This ends resizing mode
  stopTimer();
  const auto bounds = getLocalBounds();
  const auto width = bounds.getWidth();
  const auto height = bounds.getHeight();

  // First check if the size increased or decreased
  bool biggerWidth = width > lastWidth;
  bool biggerHeight = height > lastHeight;
  bool bigger = biggerWidth || biggerHeight;

  // We check the current ratio against the target ratio
  // Our goal is to keep the aspect ratio of the panel constant
  // while just removing the excess space
  const auto currentRatio = (float)width / (float)height;
  if (bigger) {
    if (currentRatio < ratio) {
      const auto newWidth = (int)(height * ratio);
      setSize(newWidth, height);
    } else {
      const auto newHeight = (int)(width / ratio);
      setSize(width, newHeight);
    }
  } else {
    if (currentRatio > ratio) {
      const auto newWidth = (int)(height * ratio);
      setSize(newWidth, height);
    } else {
      const auto newHeight = (int)(width / ratio);
      setSize(width, newHeight);
    }
  }

  // We update the last width and height
  lastWidth = getWidth();
  lastHeight = getHeight();

  size = (float)getHeight() / (float)baseHeight;
  disfluxPanel.setBounds(bounds);
  addAndMakeVisible(disfluxPanel);
  isResizing = false;
}

//==============================================================================
void
PluginEditor::paint(juce::Graphics& g)
{
  TRACER("PluginEditor::paint");

  // In resizing mode we need to paint a resized version of the cached image
  if (isResizing) {
    const int width = jmax(1, getWidth());
    const int height = jmax(1, getHeight());
    g.drawImageWithin(
      image, 0, 0, width, height, RectanglePlacement::fillDestination, false);
    return;
  }

  // Just painting the background
  g.fillAll(dmt::Settings::Colours::background);
}

//==============================================================================
void
PluginEditor::resized()
{
  TRACER("PluginEditor::resized");

  // Because JUCE is to stupid to handle resize limits without laggig we do this
  // if (getHeight() < 100) {
  //   setBounds(getBounds().withHeight(100));
  // }
  // if (getWidth() < 200) {
  //   setBounds(getBounds().withWidth(200));
  // }

  // Let's cache this component's graphics to an image
  if (!isResizing) {
    const int width = jmax(1, getWidth());
    const int height = jmax(1, getHeight());
    image = Image(PixelFormat::RGB, width, height, true);
    juce::Graphics graphics(image);
    paintEntireComponent(graphics, false);
  }

  // We go into resizing mode
  startTimer(300);
  isResizing = true;

  // We remove all children to improve resize performance
  removeAllChildren();
}
void
PluginEditor::parentSizeChanged()
{
  TRACER("PluginEditor::parentSizeChanged");
}