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
 * SoftwareDisplayRenderer provides CPU-based software rendering via
 * juce::Graphics. Implements the DisplayRenderer strategy interface.
 * Use this when OpenGL is unavailable or when simple 2D drawing suffices.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/display/DisplayRenderer.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

/**
 * @brief Software-based rendering engine.
 *
 * @details
 * All drawing is performed through juce::Graphics callbacks provided
 * by the shell. No OpenGL context is required. Subclasses override
 * renderContent to define what appears in the display area.
 */
class SoftwareDisplayRenderer : public DisplayRenderer
{
public:
  //==========================================================================
  virtual ~SoftwareDisplayRenderer() = default;

  //==========================================================================
  /**
   * @brief Draws the audio visualization content.
   *
   * @param _g The graphics context
   * @param _bounds The rectangle to draw within
   *
   * @details
   * Subclasses override this to perform their software rendering.
   * Default implementation clears the background.
   */
  virtual void renderContent(juce::Graphics& _g, const juce::Rectangle<int>& _bounds)
  {
    // Default: clear background only
    _g.fillAll();
  }

  //==========================================================================
  bool requiresOpenGL() const override { return false; }
};

//==============================================================================

} // namespace display
} // namespace gui
} // namespace dmt

//==============================================================================