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
 * DisplayRenderer defines an abstract interface for the Strategy Pattern
 * used by display components to delegate their actual audio data rendering.
 * Concrete implementations (OpenGL, Software) can be swapped at runtime
 * without affecting the Display Shell.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

/**
 * @brief Abstract rendering strategy for display content.
 *
 * @details
 * This interface decouples the rendering backend from the display shell.
 * The Shell (e.g., ImpulseResponseDisplay) owns layout, labels, chrome,
 * and data. The Renderer is responsible solely for drawing the audio
 * visualization content.
 *
 * Implementations:
 *   - OpenGLDisplayRenderer: GPU-accelerated rendering via OpenGL
 *   - SoftwareDisplayRenderer: CPU-based software rendering via juce::Graphics
 */
class DisplayRenderer
{
public:
  virtual ~DisplayRenderer() = default;

  //==========================================================================
  /**
   * @brief Called when a new OpenGL context is available.
   *
   * @details
   * Override this to initialize shaders, buffers, or any GPU resources.
   * Default implementation does nothing (for software renderers).
   */
  virtual void newOpenGLContextCreated(juce::OpenGLContext&) {}

  //==========================================================================
  /**
   * @brief Called to perform the actual rendering into the OpenGL context.
   *
   * @details
   * Override this to issue draw calls. Default implementation does nothing.
   */
  virtual void render() {}

  //==========================================================================
  /**
   * @brief Called when the OpenGL context is being destroyed.
   *
   * @details
   * Override this to release GPU resources (shaders, buffers, textures).
   * Default implementation does nothing.
   */
  virtual void openGLContextClosing(juce::OpenGLContext&) {}

  //==========================================================================
  /**
   * @brief Called when the display bounds have changed.
   *
   * @details
   * Override this to update any size-dependent state.
   */
  virtual void resized(const juce::Rectangle<int>&) {}

  //==========================================================================
  /**
   * @brief Called before each frame to update data or state.
   *
   * @details
   * Use this to prepare buffers, transform data, or update uniforms
   * that will be used in the next render call.
   */
  virtual void prepareNextFrame() {}

  //==========================================================================
  /**
   * @brief Returns true if this renderer requires an OpenGL context.
   *
   * @details
   * Software renderers should return false, allowing the shell to
   * skip OpenGL context creation entirely.
   */
  virtual bool requiresOpenGL() const { return false; }
};

//==============================================================================

} // namespace display
} // namespace gui
} // namespace dmt

//==============================================================================