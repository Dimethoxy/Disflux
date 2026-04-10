#pragma once

#include "./AbstractPluginProcessor.h"
#include "gui/window/Compositor.h"
#include <JuceHeader.h>
#include <functional>

namespace dmt {
namespace app {
class AbstractPluginEditor
  : public juce::AudioProcessorEditor
  , protected juce::Timer
{
  using Image = juce::Image;
  using ImageComponent = juce::ImageComponent;
  using PixelFormat = juce::Image::PixelFormat;
  using OpenGLContext = juce::OpenGLContext;

public:
  // Strategy function type for layout initialization
  using LayoutInitializer = std::function<void(dmt::gui::window::Layout&)>;

  AbstractPluginEditor(dmt::app::AbstractPluginProcessor& _p,
                       juce::String _name,
                       int _baseWidth,
                       int _baseHeight,
                       LayoutInitializer&& _layoutInit)
    : juce::AudioProcessorEditor(&_p)
    , p(_p)
    , baseWidth(_baseWidth)
    , baseHeight(_baseHeight)
    , sizeFactor(p.sizeFactor)
    , mainLayout({}, {})
    , compositor(_name, mainLayout, p.apvts, p.properties, sizeFactor)
    , compositorAttached(true)
  {
    // Initialize the layout via strategy function
    _layoutInit(mainLayout);

    // Now that layout is fully configured, attach the compositor
    addAndMakeVisible(compositor);

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
    setResizable(false, true);

    const auto startWidth = baseWidth * sizeFactor;
    const auto startHeight = (baseHeight + headerHeight) * sizeFactor;
    setSize(startWidth, startHeight);

    // Set the callback for header visibility changes
    compositor.setHeaderVisibilityCallback([this](bool isHeaderVisible) {
      handleHeaderVisibilityChange(isHeaderVisible);
    });
  }

  ~AbstractPluginEditor() override = default;

  //==============================================================================
  //

  void paint(juce::Graphics& g)
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

  void resized()
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

  void setConstraints(int width, int height)
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

  void handleHeaderVisibilityChange(bool isHeaderVisible)
  {
    const int adjustedHeight =
      isHeaderVisible ? baseHeight + headerHeight : baseHeight;
    setConstraints(baseWidth, adjustedHeight);
    setSize(baseWidth * sizeFactor, adjustedHeight * sizeFactor);
  }

  dmt::gui::window::Layout& getMainLayout() { return mainLayout; }

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

  //==============================================================================
  // OpenGL debug overwrite

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

protected:
  dmt::app::AbstractPluginProcessor& p;

  const int& headerHeight = dmt::Settings::Header::height;
  const int baseWidth;
  const int baseHeight;
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

  OpenGLContext openGLContext;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractPluginEditor)
};
} // namespace app
} // namespace dmt