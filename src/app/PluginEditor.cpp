#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
PluginEditor::PluginEditor(PluginProcessor& p)
  : AudioProcessorEditor(&p)
  , p(p)
  , disfluxPanel(p.apvts)
  , compositor("DisFlux", disfluxPanel)
{
  if (OS_IS_WINDOWS) {
    setResizable(true, true);
  }

  if (OS_IS_DARWIN) {
    setResizable(true, true);
  }

  if (OS_IS_LINUX) {
    // openGLContext.setComponentPaintingEnabled(true);
    // openGLContext.setContinuousRepainting(false);
    // openGLContext.attachTo(*getTopLevelComponent());
    // setResizable(true, true);
  }

  if (auto* constrainer = getConstrainer()) {
    const auto aspectRatio = (double)baseWidth / (double)baseHeight;
    constrainer->setFixedAspectRatio(aspectRatio);
    const auto minWidth = baseWidth / 2;
    const auto minHeight = baseHeight / 2;
    const auto maxWidth = baseWidth * 4;
    const auto maxHeight = baseHeight * 4;
    constrainer->setSizeLimits(minWidth, minHeight, maxWidth, maxHeight);
  }

  addAndMakeVisible(compositor);
  setResizable(true, true);
  setSize(baseWidth, baseHeight);

  juce::URL url("http://www.google.com");
  juce::WebInputStream webStream(url, false);
  webStream.withExtraHeaders("User-Agent: Dimethoxy Audio Plugin");
  webStream.withConnectionTimeout(5000);
  auto response = webStream.readEntireStreamAsString();
  if (webStream.connect(nullptr)) {
    // Check the status code to see if the request was successful
    int statusCode = webStream.getStatusCode();
    if (statusCode == 200) {
      // Read the response data
      juce::MemoryBlock responseData;
      while (!webStream.isExhausted()) {
        char buffer[1024];
        int bytesRead = webStream.read(buffer, sizeof(buffer));
        responseData.append(buffer, bytesRead);
      }
      // Print out the fetched data (optional)
      DBG("Fetched data: " << responseData.toString());
    } else {
      DBG("Failed to fetch the page. Status code: " << statusCode);
    }
  } else {
    DBG("Failed to connect to the URL");
  }
}
//==============================================================================
PluginEditor::~PluginEditor() {}

//==============================================================================
void
PluginEditor::paint(juce::Graphics& g)
{
  TRACER("PluginEditor::paint");

  // Just painting the background
  g.fillAll(dmt::Settings::Window::backroundColour);
}

//==============================================================================
void
PluginEditor::resized()
{
  TRACER("PluginEditor::resized");

  // Set the global size
  float newSize = (float)getHeight() / (float)baseHeight;

  // Make sure the size makes sense
  if (newSize <= 0.0f || std::isinf(newSize)) {
    jassertfalse;
  }

  dmt::Settings::Window::size = newSize;

  // Set the bounds of the compositor to the bounds of the PluginEditor
  compositor.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
}