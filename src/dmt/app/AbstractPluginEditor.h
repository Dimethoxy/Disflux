#pragma once

#include "./AbstractPluginProcessor.h"
#include "gui/window/Compositor.h"
#include <JuceHeader.h>
#include <functional>

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

namespace dmt {
namespace app {
class AbstractPluginEditor
  : public juce::AudioProcessorEditor
  , protected juce::Timer
{
public:
  // Strategy function type for layout initialization
  using LayoutInitializer = std::function<void(dmt::gui::window::Layout&)>;

  AbstractPluginEditor(dmt::app::AbstractPluginProcessor& p,
                       LayoutInitializer&& layoutInit)
    : juce::AudioProcessorEditor(&p)
    , sizeFactor(p.sizeFactor)
    , mainLayout({}, {})
    , compositor("DisFlux", mainLayout, p.apvts, p.properties, sizeFactor)
    , compositorAttached(true)
  {
    // Initialize the layout via strategy function
    layoutInit(mainLayout);

    // Now that layout is fully configured, attach the compositor
    addAndMakeVisible(compositor);
  }

  ~AbstractPluginEditor() override = default;

  //==============================================================================
  // Debounced resizing

  // Debounce timer callback: reattach compositor and repaint
  void timerCallback() override
  {
    stopTimer();
    attachCompositorAfterResize();
    repaint(); // TODO: Redundanr call, maybe remove this?
  }

  // Detach compositor to improve resize performance
  void detachCompositorForResize()
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

  // Reattach compositor and repaint after resizing
  void attachCompositorAfterResize()
  {
    if (!compositorAttached) {
      // Snap to the correct aspect ratio, considering header visibility
      auto bounds = getLocalBounds();
      bool headerVisible = compositor.isHeaderVisible();
      int aspectHeight =
        headerVisible ? (baseHeight + headerHeight) : baseHeight;
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

  // Capture the current compositor state into an image for smooth resizing
  void updateCompositorSnapshot()
  {
    // Render compositor to an image at its current size
    if (getWidth() > 0 && getHeight() > 0) {
      compositorSnapshot =
        juce::Image(juce::Image::ARGB, getWidth(), getHeight(), true);
      juce::Graphics g(compositorSnapshot);
      compositor.paintEntireComponent(g, true);
    }
  }

  dmt::gui::window::Layout& getMainLayout() { return mainLayout; }

protected:
  const int& headerHeight = dmt::Settings::Header::height;
  const int baseWidth = 500;
  const int baseHeight = 270;
  int lastWidth = baseWidth;
  int lastHeight = baseHeight;
  double ratio = baseWidth / baseHeight;
  float& sizeFactor;
  bool firstDraw = true;

  juce::Image compositorSnapshot;
  bool compositorAttached = true;

  Image image;
  bool isResizing = false;

  dmt::gui::window::Layout mainLayout;
  dmt::gui::window::Compositor compositor;
};
} // namespace app
} // namespace dmt