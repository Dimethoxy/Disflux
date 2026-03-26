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
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * Shadow component for JUCE, supporting both inner and outer drop shadows
 * for arbitrary paths. Designed for real-time GUI rendering.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "utility/Scaleable.h"
#include "utility/Settings.h"
#include <JuceHeader.h>
#include <melatonin_blur/melatonin_blur.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================
/**
 * @brief Component for rendering drop shadows on arbitrary paths.
 *
 * @details
 * This class provides a reusable JUCE component for drawing both inner and
 * outer drop shadows on any path. It is optimized for real-time GUI use,
 * caching the shadow image and only repainting when necessary. The shadow
 * can be toggled visible/invisible, and supports dynamic resizing and path
 * changes. Designed for use in custom widgets and panels.
 */
class Shadow
  : public juce::Component
  , public dmt::Scaleable<Shadow>
{
  //==============================================================================
  // Alias for convenience
  using Image = juce::Image;
  using Graphics = juce::Graphics;
  using String = juce::String;
  using PixelFormat = juce::Image::PixelFormat;

public:
  /**
   * @brief Constructs a Shadow component.
   *
   * @param _visibility Whether the shadow is visible.
   * @param _colour The colour of the shadow.
   * @param _radius The blur radius for the shadow.
   * @param _inner If true, draws an inner shadow; otherwise, draws an outer
   * shadow.
   *
   * @details
   * The constructor is constexpr for macOS compatibility. All parameters are
   * stored as references or values for maximum efficiency and flexibility.
   */
  explicit Shadow(const bool& _visibility,
                  const juce::Colour& _colour,
                  const float& _radius,
                  const bool _inner) noexcept
    : visibility(_visibility)
    , colour(&_colour)
    , radius(_radius)
    , inner(_inner)
  {
  }

  //==============================================================================
  /**
   * @brief Paints the shadow image onto the component.
   *
   * @param _g The graphics context.
   *
   * @details
   * If the shadow is not visible, nothing is drawn. If the cached image is
   * valid, it is drawn directly for performance. Otherwise, the shadow is
   * re-rendered to the image and then drawn.
   */
  inline void paint(juce::Graphics& _g) override
  {
    TRACER("Shadow::paint");

    if (!visibility)
      return;

    refreshCachedImageIfNeeded();

    if (!needsRepaint) {
      _g.drawImage(image,
                   0.0f,
                   0.0f,
                   static_cast<float>(getWidth()),
                   static_cast<float>(getHeight()),
                   0,
                   0,
                   image.getWidth(),
                   image.getHeight());
      return;
    }

    juce::Graphics imageGraphics(image);
    imageGraphics.addTransform(juce::AffineTransform::scale(scale, scale));
    imageGraphics.fillAll(juce::Colours::transparentBlack);
    imageGraphics.setColour(*colour); // dereference pointer

    if (inner)
      drawInnerForPath(imageGraphics, path);
    else
      drawOuterForPath(imageGraphics, path);

    needsRepaint = false;
    _g.drawImage(image,
                 0.0f,
                 0.0f,
                 static_cast<float>(getWidth()),
                 static_cast<float>(getHeight()),
                 0,
                 0,
                 image.getWidth(),
                 image.getHeight());
  }

  //==============================================================================
  /**
   * @brief Handles resizing of the shadow component.
   *
   * @details
   * When the component is resized, the shadow image is recreated to match
   * the new dimensions, and a repaint is triggered. If the new size is zero,
   * nothing is done.
   */
  inline void resized() override
  {
    TRACER("Shadow::resized");
    refreshCachedImageIfNeeded(true);
  }

  //==============================================================================
  /**
   * @brief Changes the Colour of the shadow.
   *
   * @param _newColour The new colour to use for the shadow.
   *
   * @details
   * Updates the internal colour and triggers a repaint to update the shadow
   * image.
   */
  inline void setColour(const juce::Colour& _newColour)
  {
    TRACER("Shadow::setColour");
    colour = &_newColour;
    needsRepaint = true;
    repaint();
  }

  //==============================================================================
  /**
   * @brief Sets the path for which the shadow is rendered.
   *
   * @param _newPath The new path to use for the shadow.
   *
   * @details
   * Updates the internal path and triggers a resize to update the shadow image.
   */
  inline void setPath(juce::Path _newPath)
  {
    TRACER("Shadow::setPath");
    path = _newPath;
    resized();
  }

  //==============================================================================
  /**
   * @brief Directly draws the shadow for the given path.
   *
   * @param _g The graphics context.
   * @param _target The path to shadow.
   *
   * @details
   * This method is used if the shadow isn't used as a component, allowing
   * for direct drawing of the shadow on a given graphics context.
   * It doesn't do any caching or image management, so it's suitable for one-off
   * rendering tasks.
   */
  inline void directDraw(juce::Graphics& _g, juce::Path _target)
  {
    TRACER("Shadow::directDraw");
    if (inner)
      drawInnerForPath(_g, _target);
    else
      drawOuterForPath(_g, _target);
  }

protected:
  //==============================================================================
  /**
   * @brief Draws an inner drop shadow for the given path.
   *
   * @param _g The graphics context.
   * @param _target The path to shadow.
   *
   * @details
   * Uses JUCE's DropShadow to render an inner shadow by clipping to the
   * target path and drawing the shadow on an expanded rectangle.
   */
  inline void drawInnerForPath(juce::Graphics& _g, juce::Path _target)
  {
    TRACER("Shadow::drawInnerForPath");
    updateShadowParameters();
    innerShadowRenderer.render(_g, _target);
  }

  //==============================================================================
  /**
   * @brief Draws an outer drop shadow for the given path.
   *
   * @param _g The graphics context.
   * @param _target The path to shadow.
   *
   * @details
   * Uses JUCE's DropShadow to render an outer shadow by clipping to the
   * expanded shadow path and drawing the shadow on the original path.
   */
  inline void drawOuterForPath(juce::Graphics& _g, juce::Path _target)
  {
    TRACER("Shadow::drawOuterForPath");
    juce::Graphics::ScopedSaveState saveState(_g);
    juce::Path shadowPath(_target);
    shadowPath.addRectangle(_target.getBounds().expanded(10 * scale));
    shadowPath.setUsingNonZeroWinding(false);
    _g.reduceClipRegion(shadowPath);
    updateShadowParameters();
    outerShadowRenderer.render(_g, _target);
  }

private:
  inline void updateShadowParameters()
  {
    const auto scaledRadius = static_cast<double>(radius * size * scale);
    const auto scaledOffset = juce::Point<float>(
      static_cast<float>(offset.x) * static_cast<float>(scale),
      static_cast<float>(offset.y) * static_cast<float>(scale));

    if (inner) {
      innerShadowRenderer.setColor(*colour).setRadius(scaledRadius).setOffset(
        scaledOffset);
      return;
    }

    outerShadowRenderer.setColor(*colour).setRadius(scaledRadius).setOffset(
      scaledOffset);
  }

  inline void refreshCachedImageIfNeeded(bool forceRepaint = false)
  {
    if (getWidth() == 0 || getHeight() == 0)
      return;

    const auto currentScale = static_cast<float>(scale);
    const auto scaledWidth =
      juce::jmax(1, juce::roundToInt(getWidth() * currentScale));
    const auto scaledHeight =
      juce::jmax(1, juce::roundToInt(getHeight() * currentScale));

    const auto scaleChanged =
      !juce::approximatelyEqual(lastRenderedScale, currentScale);
    const auto imageSizeChanged =
      image.getWidth() != scaledWidth || image.getHeight() != scaledHeight;

    if (!forceRepaint && !scaleChanged && !imageSizeChanged)
      return;

    image = Image(PixelFormat::ARGB, scaledWidth, scaledHeight, true);
    lastRenderedScale = currentScale;
    needsRepaint = true;
  }

  //==============================================================================
  // Members initialized in the initializer list
  const bool& visibility;
  const juce::Colour* colour; // pointer instead of reference
  const float& radius;
  const bool inner;

  //==============================================================================
  // Other members
  juce::Point<int> offset = { 0, 0 };
  juce::Path path;
  bool needsRepaint = true;
  float lastRenderedScale = 0.0f;

  melatonin::DropShadow outerShadowRenderer;
  melatonin::InnerShadow innerShadowRenderer;

  Image image = Image(PixelFormat::ARGB, 1, 1, true);

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Shadow)
};

//==============================================================================
} // namespace widget
} // namespace gui
} // namespace dmt