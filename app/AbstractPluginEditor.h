#pragma once

//==============================================================================
// Preprocessor flags for renderer control
#define DMT_SUPPRESS_GL_DEBUG_MESSAGES 0

//==============================================================================

#include "app/AbstractPluginProcessor.h"
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
  // Window sizing mode
  enum class WindowMode
  {
    Fixed,
    Dynamic
  };

  // Configuration for window sizing behavior
  struct WindowConfig
  {
    int baseWidth;
    int baseHeight;
    WindowMode mode = WindowMode::Fixed;
    float minSizeMultiplier = 0.5f;
    float maxSizeMultiplier = 2.0f;
    float minAspectRatio = 0.5f;
    float maxAspectRatio = 2.0f;
    int dynamicStartWidth = -1;
    int dynamicStartHeight = -1;
  };

  // Custom constrainer that enforces aspect ratio bounds (min/max aspect
  // ratios)
  class AspectRatioBoundsConstrainer : public juce::ComponentBoundsConstrainer
  {
  public:
    AspectRatioBoundsConstrainer(float minRatio,
                                 float maxRatio,
                                 int baseW,
                                 int baseH,
                                 int headerH)
      : minAspectRatio(minRatio)
      , maxAspectRatio(maxRatio)
      , baseWidth(baseW)
      , baseHeight(baseH)
      , headerHeight(headerH)
    {
    }

    void checkBounds(juce::Rectangle<int>& bounds,
                     const juce::Rectangle<int>& previousBounds,
                     const juce::Rectangle<int>& limits,
                     bool isStretchingTop,
                     bool isStretchingLeft,
                     bool isStretchingBottom,
                     bool isStretchingRight) override
    {
      // First apply default size limits
      ComponentBoundsConstrainer::checkBounds(bounds,
                                              previousBounds,
                                              limits,
                                              isStretchingTop,
                                              isStretchingLeft,
                                              isStretchingBottom,
                                              isStretchingRight);

      // Then enforce aspect ratio bounds
      int w = bounds.getWidth();
      int h = bounds.getHeight();
      double currentRatio = (double)w / (double)h;

      if (currentRatio < minAspectRatio) {
        // Too narrow - increase width
        w = static_cast<int>(h * minAspectRatio);
      } else if (currentRatio > maxAspectRatio) {
        // Too wide - decrease width
        w = static_cast<int>(h * maxAspectRatio);
      }

      bounds.setSize(w, h);
    }

  private:
    float minAspectRatio, maxAspectRatio;
    int baseWidth, baseHeight, headerHeight;
  };

  // Strategy function type for layout initialization
  using LayoutInitializer = std::function<void(dmt::gui::window::Layout&)>;

  // Primary constructor with WindowConfig
  AbstractPluginEditor(dmt::app::AbstractPluginProcessor& _p,
                       juce::String _name,
                       const WindowConfig& _windowConfig,
                       LayoutInitializer&& _layoutInit)
    : juce::AudioProcessorEditor(&_p)
    , p(_p)
    , baseWidth(_windowConfig.baseWidth)
    , baseHeight(_windowConfig.baseHeight)
    , windowMode(_windowConfig.mode)
    , minSizeMultiplier(_windowConfig.minSizeMultiplier)
    , maxSizeMultiplier(_windowConfig.maxSizeMultiplier)
    , minAspectRatio(_windowConfig.minAspectRatio)
    , maxAspectRatio(_windowConfig.maxAspectRatio)
    , sizeFactor(p.sizeFactor)
    , mainLayout({}, {})
    , compositor(_name, mainLayout, p.apvts, p.properties, sizeFactor)
    , compositorAttached(true)
  {
    // Initialize the layout via strategy function
    _layoutInit(mainLayout);

    // Now that layout is fully configured, attach the compositor
    addAndMakeVisible(compositor);

#if OS_IS_DARWIN || OS_IS_LINUX
    // Determine if hardware acceleration should be used
    bool shouldUseOpenGL = dmt::Settings::useOpenGL;
    DBG("[AbstractPluginEditor] OpenGL renderer: "
        << (shouldUseOpenGL ? "ENABLED" : "DISABLED"));

    if (OS_IS_DARWIN) {
      // macOS: Use OpenGL for hardware acceleration if enabled
      if (shouldUseOpenGL) {
        DBG("[AbstractPluginEditor] Using macOS OpenGL renderer");
        openGLContext.setComponentPaintingEnabled(true);
        openGLContext.setContinuousRepainting(false);
        openGLContext.attachTo(*getTopLevelComponent());
        setupOpenGLContext();
      } else {
        DBG("[AbstractPluginEditor] Using macOS software renderer");
      }
      setResizable(false, true);
    }

    if (OS_IS_LINUX) {
      // Linux: Use OpenGL for hardware acceleration if enabled
      if (shouldUseOpenGL) {
        DBG("[AbstractPluginEditor] Using Linux OpenGL renderer");
        openGLContext.setComponentPaintingEnabled(true);
        openGLContext.setContinuousRepainting(false);
        openGLContext.attachTo(*getTopLevelComponent());
        setupOpenGLContext();
      } else {
        DBG("[AbstractPluginEditor] Using Linux software renderer");
      }
    }

#endif

    // Configure window constraints based on mode
    if (windowMode == WindowMode::Dynamic) {
      // Create custom constrainer for aspect ratio bounds
      aspectRatioBoundsConstrainer =
        std::make_unique<AspectRatioBoundsConstrainer>(
          minAspectRatio,
          maxAspectRatio,
          baseWidth,
          baseHeight,
          static_cast<int>(dmt::Settings::Header::height));
      setConstrainer(aspectRatioBoundsConstrainer.get());
      activateDynamicWindowSize(minSizeMultiplier, maxSizeMultiplier);
    } else {
      setConstraints(baseWidth, baseHeight + headerHeight);
    }
    setResizable(false, true);

    // Resolve initial window size using priority order
    int initialWidth = baseWidth;
    int initialHeight = baseHeight + headerHeight;

    // Priority 1: Check for saved window state
    int savedWidth = p.getSavedWindowWidth();
    int savedHeight = p.getSavedWindowHeight();
    if (savedWidth > 0 && savedHeight > 0) {
      initialWidth = savedWidth;
      initialHeight = savedHeight;
      isHeaderHidden = p.getSavedHeaderHiddenState();
    }
    // Priority 2: Use dynamic start dimensions if in dynamic mode
    else if (windowMode == WindowMode::Dynamic &&
             _windowConfig.dynamicStartWidth > 0 &&
             _windowConfig.dynamicStartHeight > 0) {
      initialWidth = _windowConfig.dynamicStartWidth;
      initialHeight = _windowConfig.dynamicStartHeight;
    }
    // Priority 3: Use base dimensions (default, already set above)

    const auto startWidth = initialWidth * sizeFactor;
    const auto startHeight = initialHeight * sizeFactor;
    setSize(startWidth, startHeight);

    // Set the callback for header visibility changes
    compositor.setHeaderVisibilityCallback([this](bool isHeaderVisible) {
      handleHeaderVisibilityChange(isHeaderVisible);
    });
  }

  // Deprecated: Use the WindowConfig constructor instead
  [[deprecated("Use constructor with WindowConfig parameter instead")]]
  AbstractPluginEditor(dmt::app::AbstractPluginProcessor& _p,
                       juce::String _name,
                       int _baseWidth,
                       int _baseHeight,
                       LayoutInitializer&& _layoutInit)
    : AbstractPluginEditor(_p,
                           _name,
                           WindowConfig{ _baseWidth, _baseHeight },
                           std::forward<LayoutInitializer>(_layoutInit))
  {
  }

  ~AbstractPluginEditor()
  {
    // Ensure OpenGL context is detached before destruction
    if (openGLContext.isAttached()) {
      openGLContext.detach();
    }

    // Stop the debounce timer if it's running
    stopTimer();
  }

  //==============================================================================
  // JUCE overrides

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

  //==============================================================================
  // JUCE overrides

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

  void activateDynamicWindowSize(float minRatio, float maxRatio)
  {
    if (auto* constrainer = this->getConstrainer()) {
      constrainer->setFixedAspectRatio(0.0); // Allow variable aspect ratio
      // Only limit height; width is controlled by aspect ratio bounds
      const auto minHeight = static_cast<int>(baseHeight * minRatio);
      const auto maxHeight = static_cast<int>(baseHeight * maxRatio);
      // Calculate width range based on aspect ratio bounds and height range
      const auto minWidth = static_cast<int>(minHeight * minAspectRatio);
      const auto maxWidth = static_cast<int>(maxHeight * maxAspectRatio);
      constrainer->setSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
    } else {
      jassertfalse; // Constrainer not set
    }
  }

  void handleHeaderVisibilityChange(bool isHeaderVisible)
  {
    isHeaderHidden = !isHeaderVisible;
    const int adjustedHeight =
      isHeaderVisible ? baseHeight + headerHeight : baseHeight;

    if (windowMode == WindowMode::Dynamic) {
      // In dynamic mode, recalculate size limits but keep aspect ratio bounds
      if (auto* constrainer = this->getConstrainer()) {
        const auto minHeight =
          static_cast<int>(adjustedHeight * minSizeMultiplier);
        const auto maxHeight =
          static_cast<int>(adjustedHeight * maxSizeMultiplier);
        const auto minWidth = static_cast<int>(minHeight * minAspectRatio);
        const auto maxWidth = static_cast<int>(maxHeight * maxAspectRatio);
        constrainer->setSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
      }
      // Keep width the same, only adjust height by header height
      int heightAdjustment = static_cast<int>(
        (isHeaderVisible ? headerHeight : -headerHeight) * sizeFactor);
      setSize(getWidth(), getHeight() + heightAdjustment);
    } else {
      // In fixed mode, enforce fixed aspect ratio
      setConstraints(baseWidth, adjustedHeight);
      setSize(baseWidth * sizeFactor, adjustedHeight * sizeFactor);
    }

    // Save the header visibility state
    p.saveWindowState(getWidth(), getHeight(), isHeaderHidden);
  }

  dmt::gui::window::Layout& getMainLayout() { return mainLayout; }

  //==============================================================================
  // Debounced resizing

  // Debounce timer callback: reattach compositor and repaint
  void timerCallback() override
  {
    stopTimer();
    attachCompositorAfterResize();
    repaint(); // TODO: Redundant call, maybe remove this?
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
      // For fixed mode, snap to the correct aspect ratio
      // For dynamic mode, keep the current size as-is
      if (windowMode == WindowMode::Fixed) {
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
      }

      // Set compositor bounds to fill the editor
      addAndMakeVisible(compositor);
      compositor.setBounds(getLocalBounds());
      compositorAttached = true;
      repaint();

      // Save the current window state after size correction
      p.saveWindowState(getWidth(), getHeight(), isHeaderHidden);
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
  // OpenGL initialization

  void setupOpenGLContext()
  {
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
#if DMT_SUPPRESS_GL_DEBUG_MESSAGES
          DBG("[AbstractPluginEditor] GL debug message suppression: ENABLED");
          // Suppress low-priority GL debug messages
          if (juce::gl::glDebugMessageControl) {
            juce::gl::glDebugMessageControl(
              juce::gl::GL_DEBUG_SOURCE_API,
              juce::gl::GL_DEBUG_TYPE_OTHER,
              juce::gl::GL_DEBUG_SEVERITY_NOTIFICATION,
              0,
              nullptr,
              juce::gl::GL_FALSE);
          }
#else
          DBG("[AbstractPluginEditor] GL debug message suppression: DISABLED");
#endif
          // Set up callback for GL debug messages
          if (juce::gl::glDebugMessageCallback)
            juce::gl::glDebugMessageCallback(juceFilteredGLDebugCallback,
                                             nullptr);
        },
        true);
    }).detach();
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
  WindowMode windowMode = WindowMode::Fixed;
  float minSizeMultiplier = 0.5f;
  float maxSizeMultiplier = 2.0f;
  float minAspectRatio = 0.5f;
  float maxAspectRatio = 2.0f;
  bool isHeaderHidden = false;
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
  std::unique_ptr<AspectRatioBoundsConstrainer> aspectRatioBoundsConstrainer;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractPluginEditor)
};
} // namespace app
} // namespace dmt