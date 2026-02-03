//==============================================================================
/**

 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ██   ██ ██    ██   ███     ████
 * ██   ██ ██ ██  ██  ██ ██         ██    ██   ██ ██    ██  ██ ██     ██
 * ██████  ██ ██      ██ ███████    ██    ██   ██  ██████  ██   ██    ██
 *
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * This file is part of the Dimethoxy Library, a collection of essential
 * classes used across various Dimethoxy projects.
 * These files are primarily designed for internal use within our repositories.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * The `Popover` class provides a UI component for displaying contextual
 * messages with a spike pointing to an anchor. It supports shadows, rounded
 * corners, and dynamic resizing.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/gui/widget/CallbackButton.h"
#include "dmt/utility/Math.h"
#include "dmt/utility/Scaleable.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace window {

//==============================================================================
/**
 * @brief A UI component for displaying contextual messages with a spike.
 *
 * The `Popover` class is used to display messages anchored to a specific
 * point on the screen. It supports customizable shadows, rounded corners,
 * and dynamic resizing based on its content.
 *
 * @note This component should be set to the entire windows size as it will pass
 * mouse clicks through to the components below it if the mouse doesn't hover
 * over the actual popover part.
 */
class Popover
  : public juce::Component
  , public dmt::Scaleable<Popover>
{
  //==============================================================================
  // Aliases
  using CallbackButton = dmt::gui::widget::CallbackButton;
  using String = juce::String;
  using Rectangle = juce::Rectangle<int>;
  using Colour = juce::Colour;
  using Shadow = dmt::gui::widget::Shadow;
  using Label = dmt::gui::widget::Label;
  using Fonts = dmt::utility::Fonts;

  using Settings = dmt::Settings;
  using Layout = dmt::Settings::Window;
  using PopoverSettings = Settings::Popover;

  //==============================================================================
  // Popover
  const Colour backgroundColour = PopoverSettings::backgroundColour;
  const Colour borderColour = PopoverSettings::borderColour;
  const Colour textColour = PopoverSettings::textColour;
  const Colour innerShadowColour = PopoverSettings::innerShadowColour;
  const Colour outerShadowColour = PopoverSettings::outerShadowColour;
  const Colour titleFontColour = PopoverSettings::titleFontColour;
  const Colour messageFontColour = PopoverSettings::messageFontColour;

  const float& rawCornerRadius = PopoverSettings::rawCornerRadius;
  const float& rawBorderWidth = PopoverSettings::rawBorderWidth;
  const float& innerShadowRadius = PopoverSettings::innerShadowRadius;
  const float& outerShadowRadius = PopoverSettings::outerShadowRadius;
  const float& titleFontSize = PopoverSettings::titleFontSize;
  const float& messageFontSize = PopoverSettings::messageFontSize;
  const bool& drawOuterShadow = PopoverSettings::drawOuterShadow;
  const bool& drawInnerShadow = PopoverSettings::drawInnerShadow;

  // TODO: This shouldn't be like this
  const int rawSurfaceWidth = 200;
  const int rawSurfaceHeight = 88;
  const float rawSpikeWidth = 20.0f;
  const float rawSpikeHeight = 20.0f;
  const int rawCloseButtonSize = 35;

public:
  //==============================================================================
  /**
   * @brief Constructs a `Popover` instance.
   *
   * Initializes the popover with default settings, including shadows,
   * labels, and a close button.
   */
  Popover() noexcept
    : outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true)
    , titleLabel("Title",
                 fonts.medium,
                 titleFontSize,
                 titleFontColour,
                 juce::Justification::topLeft)
    , messageLabel("Message",
                   fonts.light,
                   messageFontSize,
                   messageFontColour,
                   juce::Justification::topLeft,
                   true)
  {
    TRACER("Popover::Popover");
    setAlwaysOnTop(true);
    setInterceptsMouseClicks(false, true);

    addAndMakeVisible(closeButton);
    closeButton.onClick = [this] { hideMessage(); };

    addAndMakeVisible(outerShadow);
    addAndMakeVisible(innerShadow);
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(messageLabel);
  }

  //==============================================================================
  /** @brief Destructor for `Popover`. */
  ~Popover() noexcept override
  {
    TRACER("Popover::~Popover");
    setVisible(false);
  }

  //==============================================================================
  /**
   * @brief Paints the popover component.
   *
   * Draws the border, background, and optionally debug bounds for the
   * title and message labels.
   *
   * @param _g The graphics context used for painting.
   */
  void paint(juce::Graphics& _g) noexcept override
  {
    TRACER("Popover::paint");
    // Skip drawing if the anchor is null
    if (normalizedAnchor == nullptr)
      return;

    // Draw the border
    _g.setColour(borderColour);
    _g.fillPath(createPath(true));

    // Draw the background
    _g.setColour(backgroundColour);
    _g.fillPath(createPath(false));

    // Debug draw box around title and message labels
    if (dmt::Settings::debugBounds) {
      _g.setColour(juce::Colours::red);
      _g.drawRect(titleLabel.getBounds(), 1);
      _g.setColour(juce::Colours::green);
      _g.drawRect(messageLabel.getBounds(), 1);
      _g.setColour(juce::Colours::blue.withAlpha(0.2f));
      _g.drawRect(getLocalBounds(), 1);
    }
  }

  //==============================================================================
  /**
   * @brief Handles mouse hit testing for the popover.
   *
   * Ensures that the close button can be clicked while passing through
   * other mouse click events.
   *
   * @param _x The x-coordinate of the mouse click.
   * @param _y The y-coordinate of the mouse click.
   * @return `true` if the click is within the cached message bounds.
   */
  [[nodiscard]] bool hitTest(int _x, int _y) noexcept override
  {
    TRACER("Popover::hitTest");
    return cachedMessageBounds.contains(_x, _y);
  }

  //==============================================================================
  /**
   * @brief Resizes the popover and its child components.
   *
   * Dynamically adjusts the layout of the close button, shadows, and
   * labels based on the current size of the popover.
   */
  void resized() noexcept override
  {
    TRACER("Popover::resized");
    cachedMessageBounds = createMessageBounds(false);

    const auto closeButtonSize = static_cast<int>(rawCloseButtonSize * size);
    closeButton.setBounds(juce::Rectangle<int>(cachedMessageBounds)
                            .removeFromTop(closeButtonSize)
                            .removeFromRight(closeButtonSize));
    closeButton.setAlwaysOnTop(true);

    outerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    outerShadow.setPath(createPath(true));
    innerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    innerShadow.setPath(createPath(false));

    auto adjustedBounds =
      cachedMessageBounds.reduced(static_cast<int>(10.0f * size));
    adjustedBounds.setY(adjustedBounds.getY() - static_cast<int>(4.0f * size));
    adjustedBounds.setHeight(adjustedBounds.getHeight() +
                             static_cast<int>(8.0f * size));
    const auto titleBounds =
      adjustedBounds.removeFromTop(static_cast<int>(titleFontSize * size));
    titleLabel.setBounds(titleBounds);
    messageLabel.setBounds(adjustedBounds);
  }

  //==============================================================================
  /**
   * @brief Displays a message in the popover.
   *
   * @param _anchor The anchor point for the popover spike.
   * @param _title The title text to display.
   * @param _message The message text to display.
   */
  void showMessage(Point<int> _anchor,
                   juce::String _title,
                   juce::String _message) noexcept
  {
    TRACER("Popover::showMessage");
    setNormalizedAnchor(_anchor);
    cachedMessageBounds = createMessageBounds(false); // Update cached bounds
    setVisible(true);
    resized();
    titleLabel.setText(_title);
    messageLabel.setText(_message);
    repaint();
  }

  //==============================================================================
  /** @brief Hides the popover message. */
  void hideMessage() noexcept
  {
    TRACER("Popover::hideMessage");
    normalizedAnchor.reset();
    this->setVisible(false);
  }

protected:
  //==============================================================================
  /**
   * @brief Creates the path for the popover.
   *
   * Generates the path for the popover, including its spike and rounded
   * rectangle.
   *
   * @param isOuter Whether the path is for the outer shadow.
   * @return The generated path.
   */
  [[nodiscard]] juce::Path createPath(bool isOuter = true) const noexcept
  {
    TRACER("Popover::createPath");
    juce::Path path;

    const auto messageBounds = createMessageBounds(isOuter);
    const auto spikePoints = calculateSpikePoints(isOuter);

    addSpikeToPath(path, spikePoints);
    addRoundedRectangleToPath(path, messageBounds, isOuter);

    path.closeSubPath();
    return path;
  }

  //==============================================================================
  /**
   * @brief Creates the bounds for the popover message.
   *
   * Calculates the bounds for the popover message, including adjustments
   * for the border width.
   *
   * @param isOuter Whether the bounds are for the outer shadow.
   * @return The calculated bounds.
   */
  [[nodiscard]] Rectangle createMessageBounds(bool isOuter) const noexcept
  {
    TRACER("Popover::createMessageBounds");
    const int surfaceWidth = static_cast<int>(rawSurfaceWidth * size);
    const int surfaceHeight = static_cast<int>(rawSurfaceHeight * size);
    const float borderWidth = isOuter ? 0.0f : rawBorderWidth * size;

    Rectangle messageBounds;
    const auto anchor = getAnchor();
    messageBounds.setSize(surfaceWidth, surfaceHeight);
    const auto messageBoundsOffsetY =
      surfaceHeight / 2 + static_cast<int>(rawSpikeHeight * size);
    const auto messageBoundsCentreX = anchor.x;
    const auto messageBoundsCentreY = anchor.y + messageBoundsOffsetY;
    messageBounds.setCentre(messageBoundsCentreX, messageBoundsCentreY);

    if (!isOuter)
      messageBounds = messageBounds.reduced(static_cast<int>(borderWidth));

    return messageBounds;
  }

  //==============================================================================
  /**
   * @brief Calculates the points for the popover spike.
   *
   * Determines the points for the spike, including adjustments for the
   * border width.
   *
   * @param isOuter Whether the points are for the outer shadow.
   * @return A tuple containing the spike tip, base left, and base right points.
   */
  [[nodiscard]] std::
    tuple<juce::Point<float>, juce::Point<float>, juce::Point<float>>
    calculateSpikePoints(bool isOuter) const noexcept
  {
    TRACER("Popover::calculateSpikePoints");
    const int spikeWidth = static_cast<int>(rawSpikeWidth * size);
    const int spikeHeight = static_cast<int>(rawSpikeHeight * size);
    const float borderWidth = isOuter ? 0.0f : rawBorderWidth * size;

    const auto anchor = getAnchor();
    const auto spikeTipX = anchor.x;
    const auto spikeTipY = anchor.y;
    const auto spikeBaseLeftX = anchor.x - spikeWidth / 2;
    const auto spikeBaseLeftY = anchor.y + spikeHeight;
    const auto spikeBaseRightX = anchor.x + spikeWidth / 2;
    const auto spikeBaseRightY = anchor.y + spikeHeight;

    const auto spikeTip = juce::Point<float>(spikeTipX, spikeTipY);
    const auto spikeBaseLeft =
      juce::Point<float>(spikeBaseLeftX, spikeBaseLeftY);
    const auto spikeBaseRight =
      juce::Point<float>(spikeBaseRightX, spikeBaseRightY);

    if (isOuter)
      return { spikeTip, spikeBaseLeft, spikeBaseRight };

    const auto leftSlope =
      dmt::math::calculateSlope<float, float>(spikeBaseLeft, spikeTip)
        .value_or(0.0f);
    const auto rightSlope =
      dmt::math::calculateSlope<float, float>(spikeBaseRight, spikeTip)
        .value_or(0.0f);

    const auto leftAngle = dmt::math::slopeToAngleInRadians<float>(leftSlope);
    const auto rightAngle = dmt::math::slopeToAngleInRadians<float>(rightSlope);

    const auto rotatedLeftAngle =
      leftAngle + juce::MathConstants<float>::halfPi;
    const auto rotatedRightAngle =
      rightAngle + juce::MathConstants<float>::halfPi;

    const auto marchedLeft =
      dmt::math::marchPoint(spikeBaseLeft, rotatedLeftAngle, borderWidth);
    const auto marchedRight =
      dmt::math::marchPoint(spikeBaseRight, rotatedRightAngle, borderWidth);

    const auto leftBaseProjection =
      dmt::math::projectPointToY(
        marchedLeft, leftAngle, spikeBaseLeftY + borderWidth)
        .value();

    const auto rightBaseProjection =
      dmt::math::projectPointToY(
        marchedRight, rightAngle, spikeBaseRightY + borderWidth)
        .value();

    const auto intersection =
      dmt::math::intersectInfiniteLines<float>(
        leftBaseProjection, marchedLeft, rightBaseProjection, marchedRight)
        .value();

    return { intersection, leftBaseProjection, rightBaseProjection };
  }

  //==============================================================================
  /**
   * @brief Adds the spike to the popover path.
   *
   * Appends the spike to the given path using the provided spike points.
   *
   * @param path The path to append the spike to.
   * @param spikePoints The points defining the spike.
   */
  void addSpikeToPath(
    juce::Path& path,
    const std::tuple<juce::Point<float>,
                     juce::Point<float>,
                     juce::Point<float>>& spikePoints) const noexcept
  {
    TRACER("Popover::addSpikeToPath");
    const auto& [spikeTip, spikeBaseLeft, spikeBaseRight] = spikePoints;

    path.startNewSubPath(spikeBaseLeft);
    path.lineTo(spikeTip);
    path.lineTo(spikeBaseRight);
  }

  //==============================================================================
  /**
   * @brief Adds a rounded rectangle to the popover path.
   *
   * Appends a rounded rectangle to the given path using the provided
   * message bounds.
   *
   * @param path The path to append the rectangle to.
   * @param messageBounds The bounds of the rectangle.
   * @param isOuter Whether the rectangle is for the outer shadow.
   */
  void addRoundedRectangleToPath(juce::Path& path,
                                 const Rectangle& messageBounds,
                                 bool isOuter) const noexcept
  {
    TRACER("Popover::addRoundedRectangleToPath");
    const auto topLeft = messageBounds.getTopLeft().toFloat();
    const auto topRight = messageBounds.getTopRight().toFloat();
    const auto bottomLeft = messageBounds.getBottomLeft().toFloat();
    const auto bottomRight = messageBounds.getBottomRight().toFloat();

    const auto cornerRadius =
      (rawCornerRadius - (isOuter ? 0.0f : rawBorderWidth)) * size;

    // Top flat edge
    path.lineTo(topRight.x - cornerRadius, topRight.y);

    // Top right corner
    path.addArc(topRight.x - cornerRadius,
                topRight.y,
                cornerRadius,
                cornerRadius,
                0.0f,
                juce::MathConstants<float>::halfPi);

    // Right flat edge
    path.lineTo(bottomRight.x, bottomRight.y - cornerRadius);

    // Bottom right corner
    path.addArc(bottomRight.x - cornerRadius,
                bottomRight.y - cornerRadius,
                cornerRadius,
                cornerRadius,
                juce::MathConstants<float>::halfPi,
                juce::MathConstants<float>::halfPi * 2.0f);

    // Bottom flat edge
    path.lineTo(bottomLeft.x + cornerRadius, bottomLeft.y);

    // Bottom left corner
    path.addArc(bottomLeft.x,
                bottomLeft.y - cornerRadius,
                cornerRadius,
                cornerRadius,
                juce::MathConstants<float>::halfPi * 2.0f,
                juce::MathConstants<float>::halfPi * 3.0f);

    // Left flat edge
    path.lineTo(topLeft.x, topLeft.y + cornerRadius);

    // Top left corner
    path.addArc(topLeft.x,
                topLeft.y,
                cornerRadius,
                cornerRadius,
                juce::MathConstants<float>::halfPi * 3.0f,
                juce::MathConstants<float>::halfPi * 4.0f);
  }

  //==============================================================================
  /**
   * @brief Sets the normalized anchor point for the popover.
   *
   * Converts the given anchor point to normalized coordinates based on
   * the popover's dimensions.
   *
   * @param _anchor The anchor point to normalize.
   */
  void setNormalizedAnchor(const juce::Point<int>& _anchor) noexcept
  {
    TRACER("Popover::setNormalizedAnchor");
    if (getWidth() == 0 || getHeight() == 0)
      return;

    const auto normalizedX = static_cast<float>(_anchor.x) / getWidth();
    const auto normalizedY = static_cast<float>(_anchor.y) / getHeight();
    normalizedAnchor =
      std::make_unique<juce::Point<float>>(normalizedX, normalizedY);
  }

  //==============================================================================
  /**
   * @brief Gets the anchor point for the popover.
   *
   * Converts the normalized anchor point to denormalized coordinates
   * based on the popover's dimensions.
   *
   * @return The denormalized anchor point.
   */
  [[nodiscard]] juce::Point<int> getAnchor() const noexcept
  {
    TRACER("Popover::getAnchor");
    if (normalizedAnchor == nullptr)
      return juce::Point<int>(0, 0);

    const auto denormalizedX =
      static_cast<int>(normalizedAnchor->x * getWidth());
    const auto denormalizedY =
      static_cast<int>(normalizedAnchor->y * getHeight());
    return juce::Point<int>(denormalizedX, denormalizedY);
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  Shadow outerShadow;
  Shadow innerShadow;
  Label titleLabel;
  Label messageLabel;

  //==============================================================================
  // Other members
  Fonts fonts;
  std::unique_ptr<juce::Point<float>> normalizedAnchor;
  Rectangle cachedMessageBounds;
  CallbackButton closeButton{ "CloseButton", "Close", "Close", false,
                              false,         false,   true };

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Popover)
};

} // namespace window
} // namespace gui
} // namespace dmt