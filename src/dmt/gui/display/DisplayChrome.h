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

    updateChromeOverlay(innerBounds, bounds);
  }

  virtual void resizeContent(const juce::Rectangle<int>& _contentBounds) = 0;

  //==============================================================================
  /**
   * @brief Returns a monolithic overlay image of the entire chrome.
   *
   * @details
   * The returned image contains the chrome (background + border) and both
   * shadows composed into a single image at physical pixel resolution
   * (scaled by the current `scale`). The interior of `innerBounds` is left
   * transparent, so an OpenGL display can draw this image on top of itself
   * and have the GL content show through the transparent region.
   *
   * The image is regenerated every time `resized()` runs, so it is always in
   * sync with the most recent chrome layout.
   */
  const juce::Image& getChromeOverlay() const noexcept { return chromeOverlay; }

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

  //==============================================================================
  /**
   * @brief Composes the chrome + shadows into a single overlay image.
   *
   * @param _innerBounds The inner content bounds, in logical coordinates.
   * @param _bounds The full component bounds, in logical coordinates.
   *
   * @details
   * Copies the existing `chrome` image (background + border) and the cached
   * images of the two `Shadow` components into a single ARGB image sized to
   * the full chrome area at physical pixel resolution. The interior of
   * `innerBounds` is left transparent so an OpenGL display can composite
   * this image on top of its own content.
   *
   * The shadows are refreshed synchronously via `Shadow::refreshImage()` so
   * the cached images are guaranteed to be current at composition time, even
   * though `Shadow::paint()` has not yet run on this message loop iteration.
   */
  void updateChromeOverlay(const juce::Rectangle<int>& _innerBounds,
                           const juce::Rectangle<int>& _bounds)
  {
    chromeOverlayInnerBounds = _innerBounds;

    if (_bounds.getWidth() <= 0 || _bounds.getHeight() <= 0)
      return;

    const auto hiResWidth = static_cast<int>(_bounds.getWidth() * scale);
    const auto hiResHeight = static_cast<int>(_bounds.getHeight() * scale);

    // Synchronously refresh the shadow cached images so we can copy from
    // them without having to wait for the next paint cycle.
    outerShadow.refreshImage();
    innerShadow.refreshImage();

    // Allocate (or reuse) the overlay image at the chrome's physical size.
    chromeOverlay = Image(Image::ARGB, hiResWidth, hiResHeight, true);
    chromeOverlay.clear(chromeOverlay.getBounds(),
                        juce::Colours::transparentBlack);

    juce::Graphics g(chromeOverlay);

    // Outer shadow
    if (drawOuterShadow && outerShadow.getImage().isValid())
      g.drawImage(outerShadow.getImage(),
                  0,
                  0,
                  hiResWidth,
                  hiResHeight,
                  0,
                  0,
                  outerShadow.getImage().getWidth(),
                  outerShadow.getImage().getHeight());

    // Top Chrome (background + border)
    if (chrome.isValid())
      g.drawImage(chrome,
                  0,
                  0,
                  hiResWidth,
                  hiResHeight,
                  0,
                  0,
                  chrome.getWidth(),
                  chrome.getHeight());

    // Inner shadow
    if (drawInnerShadow && innerShadow.getImage().isValid())
      g.drawImage(innerShadow.getImage(),
                  0,
                  0,
                  hiResWidth,
                  hiResHeight,
                  0,
                  0,
                  innerShadow.getImage().getWidth(),
                  innerShadow.getImage().getHeight());
  }

private:
  Shadow outerShadow;
  Shadow innerShadow;
  Image chrome;
  ImageComponent chromeComponent;
  Image chromeOverlay;
  juce::Rectangle<int> chromeOverlayInnerBounds;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisplayChrome)
};

} // namespace display
} // namespace gui
} // namespace dmt