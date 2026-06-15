//==============================================================================

#pragma once

//==============================================================================

#include "gui/widget/Shadow.h"
#include "utility/Scaleable.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

class DisplayChrome
  : public juce::Component
  , public dmt::Scaleable<DisplayChrome>
{

  using Shadow = dmt::gui::widget::Shadow;
  using Image = juce::Image;
  using ImageComponent = juce::ImageComponent;

  //==============================================================================
  // Chrome settings (moved from AbstractDisplay)
  using DisplaySettings = dmt::Settings::Display;
  const juce::Colour& backgroundColour = DisplaySettings::backgroundColour;
  const juce::Colour& displayForegroundColour =
    dmt::Settings::Panel::backgroundColour;

  // Layout
  const float& rawCornerSize = DisplaySettings::cornerSize;
  const float& rawPadding = DisplaySettings::padding;

  // Border
  const bool& drawBorder = DisplaySettings::drawBorder;
  const juce::Colour& borderColour = DisplaySettings::borderColour;
  const float& rawBorderStrength = DisplaySettings::borderStrength;

  // Shadows
  const bool& drawOuterShadow = DisplaySettings::drawOuterShadow;
  const bool& drawInnerShadow = DisplaySettings::drawInnerShadow;
  const juce::Colour& outerShadowColour = DisplaySettings::outerShadowColour;
  const juce::Colour& innerShadowColour = DisplaySettings::innerShadowColour;
  const float& outerShadowRadius = DisplaySettings::outerShadowRadius;
  const float& innerShadowRadius = DisplaySettings::innerShadowRadius;

public:
  explicit DisplayChrome() noexcept
    : outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true)
  {
    addAndMakeVisible(outerShadow);
    addAndMakeVisible(innerShadow);
    addAndMakeVisible(chromeComponent);
  }

  void paint(juce::Graphics& _g) noexcept override
  {
    // Draw background for the entire multi-display area
    _g.setColour(backgroundColour);
    _g.fillAll();
  }

  void resized() noexcept
  {
    auto bounds = getLocalBounds();
    const auto padding = rawPadding * size;
    const auto borderStrength = rawBorderStrength * size;
    const auto cornerSize = rawCornerSize * size;
    const auto outerBounds = bounds.reduced(static_cast<int>(padding));
    const auto innerBounds =
      outerBounds.reduced(static_cast<int>(borderStrength));
    const float outerCornerSize = cornerSize;
    const float innerCornerSize = std::clamp(
      outerCornerSize - (borderStrength * 0.5f), 0.0f, outerCornerSize);

    juce::Path outerShadowPath;
    outerShadowPath.addRoundedRectangle(outerBounds, outerCornerSize);
    outerShadow.setPath(outerShadowPath);
    outerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    outerShadow.toFront(false);

    juce::Path innerShadowPath;
    innerShadowPath.addRoundedRectangle(innerBounds, innerCornerSize);
    innerShadow.setPath(innerShadowPath);
    innerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    innerShadow.toFront(false);

    resizeContent(innerBounds);

    drawChrome(innerBounds,
               outerBounds,
               innerCornerSize,
               outerCornerSize,
               borderStrength);

    drawOverlay(innerBounds);
  }

  virtual void resizeContent(const juce::Rectangle<int>& _contentBounds) = 0;

  const juce::Image& getChromeOverlay() const noexcept
  {
    return chromeOverlayCropped;
  }

protected:
  void drawChrome(const juce::Rectangle<int>& _innerBounds,
                  const juce::Rectangle<int>& _outerBounds,
                  float _innerCornerSize,
                  float _outerCornerSize,
                  float _borderStrength)
  {
    const auto bounds = getLocalBounds();
    const auto midBounds =
      juce::Rectangle<int>(_outerBounds).reduced(_borderStrength * 0.5f);
    const auto hiResWidth = static_cast<int>(bounds.getWidth() * scale);
    const auto hiResHeight = static_cast<int>(bounds.getHeight() * scale);

    // Prepare a high-resolution image for the chrome
    chrome = Image(Image::ARGB, hiResWidth, hiResHeight, true);
    juce::Graphics g(chrome);
    g.addTransform(juce::AffineTransform::scale(scale, scale));

    // First Pass: Draw the background
    if (drawBorder) {
      juce::Graphics::ScopedSaveState state(g);

      juce::Path fillClippingPath;
      fillClippingPath.setUsingNonZeroWinding(false);
      fillClippingPath.addRectangle(bounds.toFloat());
      fillClippingPath.addRoundedRectangle(midBounds.toFloat(),
                                           _innerCornerSize);
      g.reduceClipRegion(fillClippingPath);
      g.setColour(displayForegroundColour);
      g.fillRect(bounds);
    }

    // Second Pass: Draw the border
    {
      juce::Graphics::ScopedSaveState state(g);

      juce::Path borderClippingPath;
      borderClippingPath.setUsingNonZeroWinding(false);
      borderClippingPath.addRectangle(bounds.toFloat());
      borderClippingPath.addRoundedRectangle(_innerBounds.toFloat(),
                                             _innerCornerSize);
      g.reduceClipRegion(borderClippingPath);
      if (drawBorder) {
        g.setColour(borderColour);
        g.fillRoundedRectangle(_outerBounds.toFloat(), _outerCornerSize);
      } else {
        g.setColour(displayForegroundColour);
        g.fillRect(bounds);
      }
    }

    chromeComponent.setImage(chrome, juce::RectanglePlacement::stretchToFit);
    chromeComponent.setBounds(bounds);
    chromeComponent.setInterceptsMouseClicks(false, false);
    chromeComponent.toFront(false);
  }

  void drawOverlay(juce::Rectangle<int> _innerBounds)
  {
    const auto bounds = getLocalBounds();
    const auto hiResWidth = int(bounds.getWidth() * scale);
    const auto hiResHeight = int(bounds.getHeight() * scale);
    const auto hiResInnerWidth = int(_innerBounds.getWidth() * scale);
    const auto hiResInnerHeight = int(_innerBounds.getHeight() * scale);
    const auto hiResInnerX = int(_innerBounds.getX() * scale);
    const auto hiResInnerY = int(_innerBounds.getY() * scale);

    chromeOverlay = Image(Image::ARGB, hiResWidth, hiResHeight, true);
    juce::Graphics g(chromeOverlay);
    g.addTransform(juce::AffineTransform::scale(scale, scale));
    chromeComponent.paintEntireComponent(g, true);
    if (drawOuterShadow)
      outerShadow.paintEntireComponent(g, true);
    if (drawInnerShadow)
      innerShadow.paintEntireComponent(g, true);

    chromeOverlayCropped = chromeOverlay.getClippedImage(juce::Rectangle<int>(
      hiResInnerX, hiResInnerY, hiResInnerWidth, hiResInnerHeight));
  }

private:
  Shadow outerShadow;
  Shadow innerShadow;
  Image chrome;
  ImageComponent chromeComponent;

  // Used to overlay over OpenGL displays
  Image chromeOverlay;
  Image chromeOverlayCropped;
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisplayChrome)
};

} // namespace display
} // namespace gui
} // namespace dmt