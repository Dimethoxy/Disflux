//==============================================================================
/* ██████╗ ██╗███╗   ███╗███████╗████████╗██╗  ██╗ ██████╗ ██╗  ██╗██╗   ██╗
 * ██╔══██╗██║████╗ ████║██╔════╝╚══██╔══╝██║  ██║██╔═══██╗╚██╗██╔╝╚██╗ ██╔╝
 * ██║  ██║██║██╔████╔██║█████╗     ██║   ███████║██║   ██║ ╚███╔╝  ╚████╔╝
 * ██║  ██║██║██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██║   ██║ ██╔██╗   ╚██╔╝
 * ██████╔╝██║██║ ╚═╝ ██║███████╗   ██║   ██║  ██║╚██████╔╝██╔╝ ██╗   ██║
 * ╚═════╝ ╚═╝╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * Part of the Dimethoxy Library, primarily intended for Dimethoxy plugins.
 * External use is permitted but not recommended.
 * No support or compatibility guarantees are provided.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project and parts of it.
 * You are not allowed to use this code in any closed-source project.
 *
 * Description:
 * ImpulseResponseDisplay is a standard juce::Component shell (via
 * AbstractDisplay). It owns the layout, labels, data, and DisplayChrome.
 * All audio-data rendering is delegated to a DisplayRenderer strategy
 * passed at construction time. This class contains zero backend-specific
 * rendering code.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/display/AbstractDisplay.h"
#include "gui/display/DisplayRenderer.h"
#include "gui/display/SoftwareDisplayRenderer.h"
#include "gui/widget/Label.h"
#include "utility/Fonts.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

/**
 * @class ImpulseResponseDisplay
 * @brief Display shell that delegates rendering to a DisplayRenderer strategy.
 *
 * @details
 * Responsibilities of this shell:
 *   - Owns the juce::Component lifecycle (paint, resized, visibility)
 *   - Manages the DisplayChrome overlay
 *   - Hosts an optional OpenGL context (only when renderer requires it)
 *   - Delegates all audio-visualization drawing to its DisplayRenderer
 *
 * Usage:
 *   // OpenGL rendering:
 *   auto renderer = std::make_unique<OpenGLDisplayRenderer>();
 *   ImpulseResponseDisplay display(chrome, std::move(renderer));
 *
 *   // Software rendering:
 *   auto renderer = std::make_unique<MySoftwareRenderer>();
 *   ImpulseResponseDisplay display(chrome, std::move(renderer));
 */
class ImpulseResponseDisplay
  : public dmt::gui::display::AbstractDisplay
  , public juce::OpenGLRenderer
{
  using Label = dmt::gui::widget::Label;
  using Fonts = dmt::utility::Fonts;
  using Settings = dmt::Settings;

public:
  //==========================================================================
  /**
   * @brief Constructs the display shell with a rendering strategy.
   *
   * @param _displayChrome Reference to the chrome overlay provider
   * @param _renderer The rendering strategy (ownership transferred)
   */
  explicit ImpulseResponseDisplay(
    const dmt::gui::display::DisplayChrome& _displayChrome,
    std::unique_ptr<dmt::gui::display::DisplayRenderer>&& _renderer)
    : displayChrome(_displayChrome)
    , renderer(std::move(_renderer))
  {
    jassert(renderer != nullptr && "renderer must not be null");

    // Only attach OpenGL context when the strategy requires it
    if (renderer->requiresOpenGL())
    {
      openGLContext.setRenderer(this);
      openGLContext.attachTo(*this);
      openGLContext.setContinuousRepainting(true);
      openGLContext.setComponentPaintingEnabled(true);
    }

    setOpaque(false);
  }

  //==========================================================================
  ~ImpulseResponseDisplay() override { openGLContext.detach(); }

  //==========================================================================
  // Component lifecycle

  void resized() noexcept override
  {
    if (renderer)
      renderer->resized(getLocalBounds());
  }

  //==========================================================================
  void paint(juce::Graphics& _g) noexcept override
  {
    TRACER("ImpulseResponseDisplay::paint");

    if (Settings::debugBounds)
    {
      _g.setColour(juce::Colours::cyan);
      _g.drawRect(getLocalBounds(), 1);
    }

    // For software renderers, delegate drawing here
    if (renderer && !renderer->requiresOpenGL())
    {
      auto* swRenderer =
        dynamic_cast<dmt::gui::display::SoftwareDisplayRenderer*>(renderer.get());
      if (swRenderer)
        swRenderer->renderContent(_g, getLocalBounds());
    }

    // Paint display chrome overlay on top
    const auto bounds = getLocalBounds();
    const auto chromeOverlay = displayChrome.getChromeOverlay();
    _g.drawImage(chromeOverlay, bounds.toFloat());
  }

  //==========================================================================
  // OpenGL callback (only active when renderer requires OpenGL)

  void newOpenGLContextCreated() override
  {
    if (renderer)
      renderer->newOpenGLContextCreated(openGLContext);
  }

  //==========================================================================
  void renderOpenGL() override
  {
    if (renderer)
      renderer->render();
  }

  //==========================================================================
  void openGLContextClosing() override
  {
    if (renderer)
      renderer->openGLContextClosing(openGLContext);
  }

  //==========================================================================
  // Frame preparation

  void prepareNextFrame() noexcept override
  {
    if (renderer)
      renderer->prepareNextFrame();
  }

  //==========================================================================
  /**
   * @brief Returns a pointer to the current rendering strategy.
   */
  DisplayRenderer* getRenderer() const { return renderer.get(); }

private:
  //==========================================================================
  // Shell members - no rendering state, only ownership and delegation
  const dmt::gui::display::DisplayChrome& displayChrome;
  std::unique_ptr<dmt::gui::display::DisplayRenderer> renderer;

  juce::OpenGLContext openGLContext;

  //==========================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImpulseResponseDisplay)
};

//==============================================================================

} // namespace display
} // namespace gui
} // namespace dmt

//==============================================================================