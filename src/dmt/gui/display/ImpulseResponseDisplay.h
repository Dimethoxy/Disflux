#pragma once

//==============================================================================

#include "gui/display/AbstractDisplay.h"
#include "gui/widget/Label.h"
// #include "utility/Settings.h"
#include "utility/Fonts.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

/**
 * @class ImpulseResponseDisplay
 * @brief Specialized display component inheriting from
 * AbstractDisplay.
 */
class ImpulseResponseDisplay
  : public dmt::gui::display::AbstractDisplay
  , public juce::OpenGLRenderer
{
  using Label = dmt::gui::widget::Label;
  using Settings = dmt::Settings;
  using Fonts = dmt::utility::Fonts;
  using Colour = juce::Colour;
  using Image = juce::Image;

public:
  explicit ImpulseResponseDisplay(
    const dmt::gui::display::DisplayChrome& _displayChrome)
    : AbstractDisplay()
    , label("Impulse Response", fonts.bold, rawFontSize, juce::Colours::white)
    , displayChrome(_displayChrome)
  {
    // addAndMakeVisible(label);
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting(true);
    openGLContext.setComponentPaintingEnabled(true);
  }

  ~ImpulseResponseDisplay() override { openGLContext.detach(); }

  void newOpenGLContextCreated() override
  {
    shaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));

    if (!shaderProgram->addVertexShader(vertexShaderSource) ||
        !shaderProgram->addFragmentShader(fragmentShaderSource) ||
        !shaderProgram->link()) {
      jassertfalse; // shader compile/link failed
    }

    positionAttributeID = openGLContext.extensions.glGetAttribLocation(
      shaderProgram->getProgramID(), "aPos");
    jassert(positionAttributeID >= 0);
    createTriangle();
  }

  void renderOpenGL() override
  {
    juce::OpenGLHelpers::clear(juce::Colours::black);
    juce::OpenGLContext::getCurrentContext()->extensions.glUseProgram(
      shaderProgram->getProgramID());
    auto& gl = openGLContext.extensions;
    gl.glEnableVertexAttribArray(positionAttributeID);
    gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
    gl.glVertexAttribPointer(positionAttributeID,
                             2,
                             juce::gl::GL_FLOAT,
                             juce::gl::GL_FALSE,
                             sizeof(float) * 2,
                             nullptr);
    juce::gl::glDrawArrays(juce::gl::GL_TRIANGLES, 0, 3);
    gl.glDisableVertexAttribArray(positionAttributeID);
  }

  void resized() noexcept override { label.setBounds(getLocalBounds()); }

  void paint(juce::Graphics& _g) noexcept override
  {
    if (Settings::debugBounds) {
      _g.setColour(juce::Colours::cyan);
      _g.drawRect(getLocalBounds(), 1);
    }

    // paint display chrome
    const auto bounds = getLocalBounds();
    const auto chromeOverlay = displayChrome.getChromeOverlay();
    _g.drawImage(chromeOverlay, bounds.toFloat());
  }

  void openGLContextClosing() override
  {
    auto& gl = openGLContext.extensions;
    shaderProgram.reset();
    gl.glDeleteBuffers(1, &vbo);
  }

protected:
  void createTriangle()
  {
    float vertices[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f };
    auto& gl = openGLContext.extensions;
    gl.glGenBuffers(1, &vbo);
    gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
    gl.glBufferData(juce::gl::GL_ARRAY_BUFFER,
                    sizeof(vertices),
                    vertices,
                    juce::gl::GL_STATIC_DRAW);
  }
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

private:
  Label label;
  Fonts fonts;
  float rawFontSize = 24.0f;

  const dmt::gui::display::DisplayChrome& displayChrome;

  juce::OpenGLContext openGLContext;
  std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;
  int positionAttributeID = -1;
  GLuint vbo = 0;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImpulseResponseDisplay)
};
} // namespace display
} // namespace gui
} // namespace dmt
