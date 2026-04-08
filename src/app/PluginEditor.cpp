#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
PluginEditor::PluginEditor(PluginProcessor& p)
  : dmt::app::AbstractPluginEditor(p, mainLayout)
  , mainLayout({}, {})
  , p(p)
{
  mainLayout.addPanel<dmt::gui::panel::DisfluxPanel<float>>(
    0, 0, 1, 1, p.apvts, p.oscilloscopeBuffer);

  if (OS_IS_WINDOWS) {
    setResizable(false, true);
  }

  if (OS_IS_DARWIN) {
    setResizable(false, true);
  }

  if (OS_IS_LINUX) {
    openGLContext.setComponentPaintingEnabled(true);
    openGLContext.setContinuousRepainting(false);
    openGLContext.attachTo(*getTopLevelComponent());
    std::thread([this]() {
      for (int i = 0; i < 200; ++i) {
        if (openGLContext.isAttached() &&
            openGLContext.getRawContext() != nullptr)
          break;
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
      }

      if (!openGLContext.isAttached() ||
          openGLContext.getRawContext() == nullptr)
        return;

      openGLContext.executeOnGLThread(
        [](juce::OpenGLContext&) {
          if (juce::gl::glDebugMessageControl) {
            juce::gl::glDebugMessageControl(
              juce::gl::GL_DEBUG_SOURCE_API,
              juce::gl::GL_DEBUG_TYPE_OTHER,
              juce::gl::GL_DEBUG_SEVERITY_NOTIFICATION,
              0,
              nullptr,
              juce::gl::GL_FALSE);
          }

          if (juce::gl::glDebugMessageCallback)
            juce::gl::glDebugMessageCallback(juceFilteredGLDebugCallback,
                                             nullptr);
        },
        true);
    }).detach();
  }

  setConstraints(baseWidth, baseHeight + headerHeight);
  addAndMakeVisible(compositor);
  setResizable(false, true);

  const auto startWidth = baseWidth * sizeFactor;
  const auto startHeight = (baseHeight + headerHeight) * sizeFactor;
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
  setSize(baseWidth * sizeFactor, adjustedHeight * sizeFactor);
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

  if (!compositorAttached && compositorSnapshot.isValid()) {
    // Draw the last compositor snapshot, scaled to fit
    auto bounds = getLocalBounds().toFloat();
    g.drawImage(
      compositorSnapshot, bounds, juce::RectanglePlacement::stretchToFit);
    return;
  }
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

  // Update the processor's scale factor
  sizeFactor = newSize;
  p.setSizeFactor(newSize);

  // Debounced resizing logic
  if (firstDraw) {
    // On first draw, skip debounce and just layout normally
    compositor.setBounds(getLocalBounds());
    firstDraw = false;
    return;
  }
  detachCompositorForResize();
}