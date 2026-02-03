//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ███████ ██    ██   ███     ████
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
 * Alerts overlay component for displaying transient notifications in the GUI.
 * Designed for real-time performance and visual consistency.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/gui/widget/Shadow.h"
#include "dmt/utility/Fonts.h"
#include "dmt/utility/Icon.h"
#include "dmt/utility/RepaintTimer.h"
#include "dmt/utility/Scaleable.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace window {

//==============================================================================
/**
 * @brief Overlay component for displaying transient alert notifications.
 *
 * @details
 * Alerts are rendered as cached images for performance. This class manages
 * their lifecycle, fade-out, and stacking. Alerts are intended for
 * non-blocking, non-modal notifications. All painting is handled internally.
 *
 * Alerts are pushed via pushAlert(), and will fade out and remove themselves
 * after a configurable duration. Designed for use in real-time GUI contexts.
 */
class Alerts
  : public juce::Component
  , public dmt::utility::RepaintTimer
  , public dmt::Scaleable<Alerts>
{
  //==============================================================================
  /**
   * @brief Rectangle alias for convenience.
   *
   * @tparam T Rectangle coordinate type.
   */
  template<typename T>
  using Rectangle = juce::Rectangle<T>;
  using Settings = dmt::Settings;
  using Fonts = dmt::utility::Fonts;
  using Shadow = dmt::gui::widget::Shadow;
  using AlertSettings = dmt::Settings::Alerts;

  //==============================================================================
  // Alerts
  const Colour& warningBackgroundColour =
    AlertSettings::warningBackgroundColour;
  const Colour& warningBorderColour = AlertSettings::warningBorderColour;
  const Colour& warningFontColour = AlertSettings::warningFontColour;
  const Colour& warningIconColour = AlertSettings::warningIconColour;
  const Colour& warningOuterShadowColour =
    AlertSettings::warningOuterShadowColour;
  const Colour& warningInnerShadowColour =
    AlertSettings::warningInnerShadowColour;

  const Colour& errorBackgroundColour = AlertSettings::errorBackgroundColour;
  const Colour& errorBorderColour = AlertSettings::errorBorderColour;
  const Colour& errorFontColour = AlertSettings::errorFontColour;
  const Colour& errorIconColour = AlertSettings::errorIconColour;
  const Colour& errorOuterShadowColour = AlertSettings::errorOuterShadowColour;
  const Colour& errorInnerShadowColour = AlertSettings::errorInnerShadowColour;

  const Colour& infoBackgroundColour = AlertSettings::infoBackgroundColour;
  const Colour& infoBorderColour = AlertSettings::infoBorderColour;
  const Colour& infoFontColour = AlertSettings::infoFontColour;
  const Colour& infoIconColour = AlertSettings::infoIconColour;
  const Colour& infoOuterShadowColour = AlertSettings::infoOuterShadowColour;
  const Colour& infoInnerShadowColour = AlertSettings::infoInnerShadowColour;

  const Colour& successBackgroundColour =
    AlertSettings::successBackgroundColour;
  const Colour& successBorderColour = AlertSettings::successBorderColour;
  const Colour& successFontColour = AlertSettings::successFontColour;
  const Colour& successIconColour = AlertSettings::successIconColour;
  const Colour& successOuterShadowColour =
    AlertSettings::successOuterShadowColour;
  const Colour& successInnerShadowColour =
    AlertSettings::successInnerShadowColour;

  const float& rawCornerRadius = AlertSettings::cornerRadius;
  const float& rawBorderWidth = AlertSettings::borderWidth;
  const float& innerShadowRadius = AlertSettings::innerShadowRadius;
  const float& outerShadowRadius = AlertSettings::outerShadowRadius;
  const float& rawFontSize = AlertSettings::titleFontSize;
  const float& rawMessageFontSize = AlertSettings::messageFontSize;
  const float& rawTextHorizontalPadding = AlertSettings::textHorizontalPadding;
  const float& rawTextVerticalPadding = AlertSettings::textVerticalPadding;
  const float& rawIconSize = AlertSettings::iconSize;
  const float& rawContentSpacing = AlertSettings::contentSpacing;
  const int& rawAlertWidth = AlertSettings::alertWidth;
  const int& rawAlertHeight = AlertSettings::alertHeight;
  const float& maxAge = AlertSettings::maxAge;           // in seconds
  const float& fadeOutTime = AlertSettings::fadeOutTime; // in seconds
  const bool& drawOuterShadow = AlertSettings::drawOuterShadow;
  const bool& drawInnerShadow = AlertSettings::drawInnerShadow;

public:
  //==============================================================================
  /**
   * @brief Alert type enumeration.
   */
  enum class AlertType
  {
    Info,
    Warning,
    Error,
    Success
  };

protected:
  //==============================================================================
  /**
   * @brief Internal data structure for a single alert.
   *
   * @details
   * Holds all state and cached image for a single alert instance.
   */
  struct AlertData
  {
    juce::String title;
    juce::String message;
    juce::String iconName;
    AlertType type;
    float age; // in seconds, used for fading out and removing alerts
    juce::Image cachedComponentImage;
  };

public:
  //==============================================================================
  /**
   * @brief Constructs the Alerts overlay component.
   *
   * @details
   * Sets up repaint timer and disables mouse interception. Alerts are
   * non-interactive overlays.
   */
  inline Alerts() noexcept
  {
    TRACER("Alerts::Alerts");
    setInterceptsMouseClicks(false, false);
    startRepaintTimer();
  }

  //==============================================================================
  /**
   * @brief Pushes a new alert to the overlay.
   *
   * @param _title The alert title.
   * @param _message The alert message.
   * @param _type The alert type.
   * @param _iconName Optional icon name override.
   *
   * @details
   * When a new alert is pushed, existing alerts are aged to fade out sooner,
   * ensuring new alerts are always visible. The alert is rendered to a cached
   * image for performance.
   */
  inline void pushAlert(const juce::String _title,
                        const juce::String _message,
                        AlertType _type,
                        const juce::String _iconName = {}) noexcept
  {
    TRACER("Alerts::pushAlert");
    // When we push a new alert, let old ones age quicker
    const auto quickAgeTarget = maxAge - fadeOutTime;
    for (int i = 0; i < static_cast<int>(alerts.size()); ++i)
      if (alerts.getReference(i).age < quickAgeTarget)
        alerts.getReference(i).age = quickAgeTarget;

    AlertData alert{ _title, _message, _iconName, _type, 0.0f };
    renderAlertToImage(alert);
    alerts.add(alert);
    repaint();
  }

  //==============================================================================
  /**
   * @brief Handles component resize events.
   *
   * @details
   * Re-renders all cached alert images to match new scaling. This ensures
   * HiDPI and scaling correctness.
   */
  inline void resized() override
  {
    TRACER("Alerts::resized");
    // Re-render all alert images on resize
    for (int i = 0; i < static_cast<int>(alerts.size()); ++i)
      renderAlertToImage(alerts.getReference(i));
    repaint();
  }

  //==============================================================================
  /**
   * @brief Paints all active alerts.
   *
   * @param _g The graphics context.
   *
   * @details
   * Alerts are drawn from bottom up, with fade-out alpha applied as they age.
   * Cached images are used for performance.
   */
  inline void paint(juce::Graphics& _g) override
  {
    TRACER("Alerts::paint");
    if (alerts.size() == 0)
      return;

    // Calculate scaled alert size and spacing
    const auto alertWidth = rawAlertWidth * size;
    const auto alertHeight = rawAlertHeight * size;
    const int spacing = static_cast<int>(8 * size);
    const int marginBottom = static_cast<int>(24 * size);

    const int totalHeight =
      static_cast<int>(alerts.size()) * static_cast<int>(alertHeight) +
      (alerts.size() > 0 ? (static_cast<int>(alerts.size()) - 1) * spacing : 0);
    const int areaWidth = getWidth();
    const int areaHeight = getHeight();
    int y = areaHeight - totalHeight - marginBottom;

    for (const auto& alert : alerts) {
      float alpha = 1.0f;
      if (alert.age > maxAge - fadeOutTime)
        alpha = juce::jlimit(0.0f, 1.0f, (maxAge - alert.age) / fadeOutTime);

      _g.setOpacity(alpha);
      int x = (areaWidth - static_cast<int>(alertWidth)) / 2;
      _g.drawImage(alert.cachedComponentImage,
                   static_cast<float>(x),
                   static_cast<float>(y),
                   static_cast<float>(alertWidth),
                   static_cast<float>(alertHeight),
                   0,
                   0,
                   alert.cachedComponentImage.getWidth(),
                   alert.cachedComponentImage.getHeight());
      y += static_cast<int>(alertHeight) + spacing;
    }
    _g.setOpacity(1.0f);
  }

  //==============================================================================
  /**
   * @brief Called periodically to update alert ages and trigger repaint.
   *
   * @details
   * Removes alerts that have exceeded their maximum age. Triggers repaint if
   * any alerts remain.
   */
  inline void repaintTimerCallback() noexcept override
  {
    TRACER("Alerts::repaintTimerCallback");
    for (int i = static_cast<int>(alerts.size()); --i >= 0;) {
      alerts.getReference(i).age += Settings::framerate / 1000.0f;
      if (alerts.getReference(i).age >= maxAge)
        alerts.remove(i);
    }
    if (alerts.size() > 0)
      repaint();
  }

protected:
  //==============================================================================
  /**
   * @brief Renders an alert to its cached image.
   *
   * @param _alert The alert data to render.
   *
   * @details
   * This function is called whenever an alert is created or resized. It
   * pre-renders the alert to an image for fast compositing.
   */
  inline void renderAlertToImage(AlertData& _alert)
  {
    TRACER("Alerts::renderAlertToImage");
    const auto alertWidth = rawAlertWidth * size;
    const auto alertHeight = rawAlertHeight * size;

    // HiDPI support: render at higher resolution
    const int hiResWidth = static_cast<int>(alertWidth * scale);
    const int hiResHeight = static_cast<int>(alertHeight * scale);

    _alert.cachedComponentImage =
      juce::Image(juce::Image::ARGB, hiResWidth, hiResHeight, true);
    juce::Graphics g(_alert.cachedComponentImage);
    g.addTransform(juce::AffineTransform::scale(scale, scale));

    // copy the colours to use
    Colour backgroundColour, borderColour, fontColour, iconColour,
      outerShadowColour, innerShadowColour;
    switch (_alert.type) {
      case AlertType::Info:
        backgroundColour = infoBackgroundColour;
        borderColour = infoBorderColour;
        fontColour = infoFontColour;
        iconColour = infoIconColour;
        outerShadowColour = infoOuterShadowColour;
        innerShadowColour = infoInnerShadowColour;
        break;
      case AlertType::Warning:
        backgroundColour = warningBackgroundColour;
        borderColour = warningBorderColour;
        fontColour = warningFontColour;
        iconColour = warningIconColour;
        outerShadowColour = warningOuterShadowColour;
        innerShadowColour = warningInnerShadowColour;
        break;
      case AlertType::Error:
        backgroundColour = errorBackgroundColour;
        borderColour = errorBorderColour;
        fontColour = errorFontColour;
        iconColour = errorIconColour;
        outerShadowColour = errorOuterShadowColour;
        innerShadowColour = errorInnerShadowColour;
        break;
      case AlertType::Success:
        backgroundColour = successBackgroundColour;
        borderColour = successBorderColour;
        fontColour = successFontColour;
        iconColour = successIconColour;
        outerShadowColour = successOuterShadowColour;
        innerShadowColour = successInnerShadowColour;
        break;
    }

    const auto iconSize = rawIconSize * size;
    const auto titleFontSize = rawFontSize * size;
    const auto messageFontSize = rawMessageFontSize * size;
    const auto titleFont = fonts.bold.withHeight(titleFontSize);
    const auto messageFont = fonts.medium.withHeight(messageFontSize);
    const auto borderWidth = rawBorderWidth * size;
    const auto cornerRadius = rawCornerRadius * size;
    const auto innerCornerRadius = cornerRadius - borderWidth;
    const auto textHorizontalPadding = rawTextHorizontalPadding * size;
    const auto textVerticalPadding = rawTextVerticalPadding * size;
    const auto alertBounds = Rectangle<float>(0, 0, alertWidth, alertHeight);
    const auto outerBounds = alertBounds.reduced(outerShadowRadius * size);
    const auto innerBounds = outerBounds.reduced(borderWidth);
    auto contentBounds =
      innerBounds.reduced(textHorizontalPadding, textVerticalPadding);

    // Draw Border
    g.setColour(borderColour);
    g.fillRoundedRectangle(outerBounds, cornerRadius);

    // Draw Background
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(innerBounds, innerCornerRadius);

    // Draw Icon
    auto icon = icons::getIcon(_alert.iconName);
    auto uniqueIconPadding = icons::getPadding(_alert.iconName) * size;
    if (icon == nullptr) {
      switch (_alert.type) {
        case AlertType::Info:
          icon = icons::getIcon("Info");
          uniqueIconPadding = icons::getPadding("Info") * size;
          break;
        case AlertType::Warning:
          icon = icons::getIcon("Warning");
          uniqueIconPadding = icons::getPadding("Warning") * size;
          break;
        case AlertType::Error:
          icon = icons::getIcon("Error");
          uniqueIconPadding = icons::getPadding("Error") * size;
          break;
        case AlertType::Success:
          icon = icons::getIcon("Success");
          uniqueIconPadding = icons::getPadding("Success") * size;
          break;
      }
    }
    const auto clonedIcon = icon->createCopy();
    const auto iconBoundsWidth = iconSize + 2 * uniqueIconPadding;
    const auto iconBounds = contentBounds.removeFromLeft(iconBoundsWidth);
    clonedIcon->replaceColour(juce::Colours::black, iconColour);
    clonedIcon->drawWithin(
      g, iconBounds, juce::RectanglePlacement::centred, 1.0f);

    // Title
    const auto contentSpacing = rawContentSpacing * size;
    const auto contentWidth = contentBounds.getWidth() - contentSpacing;
    const auto titleBoundsHeight = contentBounds.getHeight() / 2.0f;
    const auto titleBounds = contentBounds.removeFromTop(titleBoundsHeight)
                               .removeFromRight(contentWidth);
    g.setFont(titleFont);
    g.setColour(fontColour);
    g.drawFittedText(_alert.title,
                     titleBounds.toNearestInt(),
                     juce::Justification::bottomLeft,
                     1);

    // Message
    const auto messageBounds = contentBounds.removeFromRight(contentWidth);
    g.setFont(messageFont);
    g.setColour(fontColour);
    g.drawFittedText(_alert.message,
                     messageBounds.toNearestInt(),
                     juce::Justification::centredLeft,
                     1);
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  Fonts fonts;

  //==============================================================================
  // Other members
  juce::Array<AlertData> alerts; // Store active alerts

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Alerts)
};
} // namespace window
} // namespace gui
} // namespace dmt