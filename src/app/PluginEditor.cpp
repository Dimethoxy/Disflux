#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
PluginEditor::PluginEditor(PluginProcessor& p)
  : AudioProcessorEditor(&p)
  , p(p)
  , disfluxPanel(p.apvts, p.oscilloscopeBuffer)
  , compositor("DisFlux", disfluxPanel, p.apvts, p.properties, sizeFactor)
{
  if (OS_IS_WINDOWS) {
    setResizable(true, true);
  }

  if (OS_IS_DARWIN) {
    setResizable(true, true);
  }

  // if (OS_IS_LINUX) {
  //   openGLContext.setComponentPaintingEnabled(true);
  //   openGLContext.setContinuousRepainting(false);
  //   openGLContext.attachTo(*getTopLevelComponent());
  //   setResizable(true, true);
  // }

  setConstraints(baseWidth, baseHeight + headerHeight);
  addAndMakeVisible(compositor);
  setResizable(true, true);

  const auto startWidth = baseWidth * size;
  const auto startHeight = (baseHeight + headerHeight) * size;
  setSize(startWidth, startHeight);

  // Set the callback for header visibility changes
  compositor.setHeaderVisibilityCallback([this](bool isHeaderVisible) {
    handleHeaderVisibilityChange(isHeaderVisible);
  });
}

void
PluginEditor::handleHeaderVisibilityChange(bool isHeaderVisible)
{
  const int adjustedHeight =
    isHeaderVisible ? baseHeight + headerHeight : baseHeight;
  setConstraints(baseWidth, adjustedHeight);
  setSize(baseWidth * size, adjustedHeight * size);
}
//==============================================================================
PluginEditor::~PluginEditor() {}

//==============================================================================
void
PluginEditor::paint(juce::Graphics& g)
{
  TRACER("PluginEditor::paint");

  // Just painting the background
  g.fillAll(dmt::Settings::Window::backgroundColour);
}

//==============================================================================
void
PluginEditor::setConstraints(int width, int height)
{
  if (auto* constrainer = this->getConstrainer()) {
    const auto aspectRatio = (double)width / (double)height;
    constrainer->setFixedAspectRatio(aspectRatio);
    const auto minWidth = width / 2;
    const auto minHeight = height / 2;
    const auto maxWidth = width * 2;
    const auto maxHeight = height * 2;
    constrainer->setSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
  } else {
    jassertfalse; // Constrainer not set
  }
}

//==============================================================================
void
PluginEditor::resized()
{
  TRACER("PluginEditor::resized");

  // Set the global size
  const int currentHeight = getHeight();
  const float newSize =
    (float)currentHeight /
    (compositor.isHeaderVisible() ? baseHeight + headerHeight : baseHeight);

  // Make sure the size makes sense
  if (newSize <= 0.0f || std::isinf(newSize)) {
    jassertfalse;
  }

  dmt::Settings::Window::size = newSize;
  sizeFactor = newSize;

  // Set the bounds of the compositor to the bounds of the PluginEditor
  compositor.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
}
