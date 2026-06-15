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
 * OpenGLDisplayRenderer provides GPU-accelerated rendering via OpenGL.
 * Implements the DisplayRenderer strategy interface.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/display/DisplayRenderer.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

/**
 * @brief OpenGL-based rendering engine.
 *
 * @details
 * Manages shader compilation, VBO creation, and GPU draw calls.
 * The shell component delegates newOpenGLContextCreated, renderOpenGL,
 * and openGLContextClosing to this strategy.
 */
class OpenGLDisplayRenderer : public DisplayRenderer
{
public:
  //==========================================================================
  explicit OpenGLDisplayRenderer() = default;
  ~OpenGLDisplayRenderer() override = default;

  //==========================================================================
  void newOpenGLContextCreated(juce::OpenGLContext& ctx) override
  {
    shaderProgram.reset(new juce::OpenGLShaderProgram(ctx));

    if (!shaderProgram->addVertexShader(vertexShaderSource) ||
        !shaderProgram->addFragmentShader(fragmentShaderSource) ||
        !shaderProgram->link()) {
      jassertfalse; // shader compile/link failed
    }

    positionAttributeID = ctx.extensions.glGetAttribLocation(
      shaderProgram->getProgramID(), "aPos");
    jassert(positionAttributeID >= 0);

    createTriangle(ctx);
  }

  //==========================================================================
  void render() override
  {
    if (!shaderProgram) return;

    juce::OpenGLHelpers::clear(dmt::Settings::Display::backgroundColour);

    auto& gl = juce::OpenGLContext::getCurrentContext()->extensions;
    gl.glUseProgram(shaderProgram->getProgramID());

    gl.glEnableVertexAttribArray(positionAttributeID);
    gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
    gl.glVertexAttribPointer(
      positionAttributeID, 2, juce::gl::GL_FLOAT, juce::gl::GL_FALSE,
      sizeof(float) * 2, nullptr);

    juce::gl::glDrawArrays(juce::gl::GL_TRIANGLES, 0, 3);
    gl.glDisableVertexAttribArray(positionAttributeID);
  }

  //==========================================================================
  void openGLContextClosing(juce::OpenGLContext& ctx) override
  {
    auto& gl = ctx.extensions;
    shaderProgram.reset();
    gl.glDeleteBuffers(1, &vbo);
    vbo = 0;
  }

  //==========================================================================
  bool requiresOpenGL() const override { return true; }

protected:
  //==========================================================================
  /**
   * @brief Vertex and fragment shader sources.
   *
   * @details
   * Subclasses can override these to provide custom shaders.
   */
  const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;

    void main()
    {
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
  )";

  const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0, 0.6, 0.2, 1.0);
    }
  )";

  //==========================================================================
  /**
   * @brief Creates the triangle geometry and uploads it to the GPU.
   *
   * @details
   * Override this in subclasses to provide custom geometry.
   */
  virtual void createTriangle(juce::OpenGLContext& ctx)
  {
    float vertices[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f };
    auto& gl = ctx.extensions;
    gl.glGenBuffers(1, &vbo);
    gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
    gl.glBufferData(juce::gl::GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                    juce::gl::GL_STATIC_DRAW);
  }

private:
  //==========================================================================
  std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;
  int positionAttributeID = -1;
  GLuint vbo = 0;

  //==========================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLDisplayRenderer)
};

//==============================================================================

} // namespace display
} // namespace gui
} // namespace dmt

//==============================================================================