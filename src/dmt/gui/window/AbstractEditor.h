// #pragma once

// #include "gui/panel/AbstractPanel.h"
// #include "gui/window/Compositor.h"
// #include <JuceHeader.h>

// namespace dmt {
// namespace gui {
// namespace window {

// class AbstractEditor : public juce::AudioProcessorEditor
// {
//   using AbstractPanel = dmt::gui::panel::AbstractPanel;
//   float& size = dmt::Settings::Window::size;
//   const int& headerHeight = dmt::Settings::Header::height;

// public:
//   AbstractEditor(juce::String _name,
//                  juce::AudioProcessor& _processor,
//                  AbstractPanel& _mainPanel,
//                  int _baseWidth,
//                  int _baseHeight)
//     : AudioProcessorEditor(_processor)
//     , compositor(_name, _mainPanel, _processor.apvts)
//     , baseWidth(_baseWidth)
//     , baseHeight(_baseHeight)
//   {
//     TRACER("AbstractEditor::AbstractEditor");
//     if (OS_IS_WINDOWS) {
//       setResizable(true, true);
//     }

//     if (OS_IS_DARWIN) {
//       setResizable(true, true);
//     }

//     if (OS_IS_LINUX) {
//       openGLContext.setComponentPaintingEnabled(true);
//       openGLContext.setContinuousRepainting(false);
//       openGLContext.attachTo(*getTopLevelComponent());
//       setResizable(true, true);
//     }

//     setConstraints(baseWidth, baseHeight + headerHeight);
//     addAndMakeVisible(compositor);
//     setResizable(true, true);
//     setSize(baseWidth, baseHeight);

//     // Set the callback for header visibility changes
//     compositor.setHeaderVisibilityCallback([this](bool isHeaderVisible) {
//       handleHeaderVisibilityChange(isHeaderVisible);
//     });
//   }

//   ~AbstractEditor() override = default;

//   void paint(juce::Graphics& g)
//   {
//     TRACER("AbstractEditor::paint");

//     // Just painting the background
//     g.fillAll(dmt::Settings::Window::backgroundColour);
//   }

//   void resized() override
//   {
//     TRACER("AbstractEditor::resized");

//     // Set the global size
//     const int currentHeight = getHeight();
//     const float newSize =
//       (float)currentHeight /
//       (compositor.isHeaderVisible() ? baseHeight + headerHeight :
//       baseHeight);

//     // Make sure the size makes sense
//     if (newSize <= 0.0f || std::isinf(newSize)) {
//       jassertfalse;
//     }

//     dmt::Settings::Window::size = newSize;

//     // Set the bounds of the compositor to the bounds of the PluginEditor
//     compositor.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
//   }

// protected:
//   void handleHeaderVisibilityChange(bool isHeaderVisible)
//   {
//     TRACER("AbstractEditor::handleHeaderVisibilityChange");
//     const int adjustedHeight =
//       isHeaderVisible ? baseHeight + headerHeight : baseHeight;
//     setConstraints(baseWidth, adjustedHeight);
//     setSize(baseWidth, adjustedHeight);
//   }

//   void setConstraints(int width, int height)
//   {
//     TRACER("AbstractEditor::setConstraints");
//     if (auto* constrainer = this->getConstrainer()) {
//       const auto aspectRatio = (double)width / (double)height;
//       constrainer->setFixedAspectRatio(aspectRatio);
//       const auto minWidth = width / 2;
//       const auto minHeight = height / 2;
//       const auto maxWidth = width * 4;
//       const auto maxHeight = height * 4;
//       constrainer->setSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
//     } else {
//       jassertfalse; // Constrainer not set
//     }
//   }

// private:
//   dmt::gui::window::Compositor compositor;
//   int baseWidth = 0;
//   int baseHeight = 0;
//   juce::OpenGLContext openGLContext;
// };

// } // namespace window
// } // namespace gui
// } // namespace dmt