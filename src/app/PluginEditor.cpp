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

  double ratio = baseWidth / baseHeight;
  setResizeLimits(800, 140, 4000, 4000 / ratio);
  getConstrainer()->setFixedAspectRatio(ratio);
  setSize(800.0, 800.0 / ratio);
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
  const auto height = bounds.getHeight();
  size = (float)height / (float)baseHeight;
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
    g.drawImageTransformed(
      image,
      AffineTransform::scale(width / (float)image.getWidth(),
                             height / (float)image.getHeight()));
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
  if (getHeight() < 100) {
    setBounds(getBounds().withHeight(100));
  }
  if (getWidth() < 200) {
    setBounds(getBounds().withWidth(200));
  }

  // Let's cache this component's graphics to an image
  if (!isResizing) {
    const int width = jmax(1, getWidth());
    const int height = jmax(1, getHeight());
    image = Image(PixelFormat::RGB, width, height, true);
    juce::Graphics graphics(image);
    paintEntireComponent(graphics, false);
  }

  // We remove all children to improve resize performance
  removeAllChildren();

  // We go into resizing mode
  startTimer(100);
  isResizing = true;
}
void
PluginEditor::parentSizeChanged()
{
  TRACER("PluginEditor::parentSizeChanged");
}