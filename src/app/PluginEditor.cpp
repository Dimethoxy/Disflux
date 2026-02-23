#include "PluginEditor.h"
#include "PluginProcessor.h"
// Use JUCE OpenGL helpers / function pointers
#include <chrono>
#include <juce_opengl/juce_opengl.h>
#include <thread>

namespace {
// Filter out notification-level GL debug messages
static void KHRONOS_APIENTRY
juceFilteredGLDebugCallback(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar* message,
                            const void* userParam)
{
  // Ignore low-priority notifications
  if (severity == juce::gl::GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

  // Log other messages so we don't lose important info
  juce::String msg =
    (message != nullptr) ? juce::String(message) : juce::String();
  DBG("OpenGL DBG message: " << msg);

  // Keep JUCE's behaviour for serious errors
  if (type == juce::gl::GL_DEBUG_TYPE_ERROR &&
      severity == juce::gl::GL_DEBUG_SEVERITY_HIGH)
    jassertfalse;
}

} // anonymous namespace

//==============================================================================
void
PluginEditor::handleHeaderVisibilityChange(bool isHeaderVisible)
{
  const int adjustedHeight =
    isHeaderVisible ? baseHeight + headerHeight : baseHeight;
  setConstraints(baseWidth, adjustedHeight);
  setSize(baseWidth * sizeFactor, adjustedHeight * sizeFactor);
}

//==============================================================================
PluginEditor::PluginEditor(PluginProcessor& p)
  : AudioProcessorEditor(&p)
  , p(p)
  , sizeFactor(p.scaleFactor)
  , mainLayout({}, {})
  , compositor("DisFlux", mainLayout, p.apvts, p.properties, sizeFactor)
  , compositorAttached(true)
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
  p.setScaleFactor(newSize);

  // Debounced resizing logic
  static bool firstDraw = true;
  if (firstDraw) {
    // On first draw, skip debounce and just layout normally
    compositor.setBounds(getLocalBounds());
    firstDraw = false;
    return;
  }
  detachCompositorForResize();
}

void
PluginEditor::detachCompositorForResize()
{
  if (compositorAttached) {
    // Take a snapshot before detaching
    updateCompositorSnapshot();

    // Remove compositor from view
    removeChildComponent(&compositor);
    compositorAttached = false;
  }

  // Restart debounce timer (100ms)
  stopTimer();
  startTimer(100);
}

void
PluginEditor::attachCompositorAfterResize()
{
  if (!compositorAttached) {
    // Snap to the correct aspect ratio, considering header visibility
    auto bounds = getLocalBounds();
    bool headerVisible = compositor.isHeaderVisible();
    int aspectHeight = headerVisible ? (baseHeight + headerHeight) : baseHeight;
    const double aspect = (double)baseWidth / (double)aspectHeight;
    int w = bounds.getWidth();
    int h = bounds.getHeight();
    double currentAspect = (double)w / (double)h;

    if (currentAspect > aspect) {
      // Too wide, adjust width
      w = static_cast<int>(h * aspect);
    } else if (currentAspect < aspect) {
      // Too tall, adjust height
      h = static_cast<int>(w / aspect);
    }
    setSize(w, h);

    // Set compositor bounds to fill the editor
    addAndMakeVisible(compositor);
    compositor.setBounds(getLocalBounds());
    compositorAttached = true;
    repaint();
  }
}

void
PluginEditor::updateCompositorSnapshot()
{
  // Render compositor to an image at its current size
  if (getWidth() > 0 && getHeight() > 0) {
    compositorSnapshot =
      juce::Image(juce::Image::ARGB, getWidth(), getHeight(), true);
    juce::Graphics g(compositorSnapshot);
    compositor.paintEntireComponent(g, true);
  }
}

void
PluginEditor::timerCallback()
{
  // Timer expired: reattach compositor and repaint
  stopTimer();
  attachCompositorAfterResize();
  repaint();
}
