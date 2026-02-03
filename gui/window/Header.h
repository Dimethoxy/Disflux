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
 * The `Header` class manages the header section of the application window.
 * The header includes a title and several buttons for user interaction.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once
#include "dmt/gui/widget/CallbackButton.h"
#include "dmt/gui/widget/Label.h"
#include "dmt/gui/widget/Shadow.h"
#include "dmt/gui/widget/ToggleButton.h"
#include "dmt/utility/Fonts.h"
#include "dmt/utility/Scaleable.h"
#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================
namespace dmt {
namespace gui {
namespace window {

//==============================================================================
/**
 * @brief Represents the header section of the application window.
 *
 * The `Header` class is responsible for managing the layout, appearance,
 * and functionality of the header section, including buttons, shadows,
 * and the title. It ensures a responsive and visually appealing design.
 */
class Header
  : public juce::Component
  , public dmt::Scaleable<Header>
{
  //==============================================================================
  // Aliases
  using CallbackButton = dmt::gui::widget::CallbackButton;
  using ToggleButton = dmt::gui::widget::ToggleButton;
  using Label = dmt::gui::widget::Label;
  using String = juce::String;
  using Colour = juce::Colour;
  using Fonts = dmt::utility::Fonts;
  using HeaderSettings = dmt::Settings::Header;
  using PanelSettings = dmt::Settings::Panel;
  using Shadow = dmt::gui::widget::Shadow;

  //==============================================================================
  // Panel
  const Colour& outerShadowColour = PanelSettings::outerShadowColour;
  const Colour& innerShadowColour = PanelSettings::innerShadowColour;
  const float& rawBorderStrength = PanelSettings::borderStrength;
  const float& outerShadowRadius = PanelSettings::outerShadowRadius;
  const float& innerShadowRadius = PanelSettings::innerShadowRadius;
  const bool& drawOuterShadow = PanelSettings::drawOuterShadow;
  const bool& drawInnerShadow = PanelSettings::drawInnerShadow;

  // Header
  const String name = ProjectInfo::projectName;
  const Colour& backgroundColour = HeaderSettings::backgroundColour;
  const Colour& borderColor = HeaderSettings::borderColor;
  const Colour& titleFontColour = HeaderSettings::titleFontColour;
  const Colour& buttonColour = HeaderSettings::buttonColour;

  const float& titleFontSize = HeaderSettings::titleFontSize;
  const float& rawTitleOffset = HeaderSettings::titleOffset;
  const float& rawTitleButtonWidth = HeaderSettings::titleButtonWidth;
  const float& rawHeaderButtonWidth = HeaderSettings::buttonWidth;

  // Button
  const float& rawButtonPadding = dmt::Settings::Button::padding;

public:
  //==============================================================================
  /**
   * @brief Constructs a `Header` instance.
   *
   * @param _titleText The title text to display in the header.
   * @param _apvts The audio processor value tree state for parameter
   * management.
   */
  Header(juce::String _titleText, AudioProcessorValueTreeState& _apvts) noexcept
    : outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, innerShadowColour, innerShadowRadius, true)
    , title(String("ProjectLabel"),
            fonts.display,
            titleFontSize,
            titleFontColour,
            juce::Justification::centred)
    , settingsButton("HeaderSettingsButton", "Settings", "Open settings")
    , settingsExitButton("HeaderSettingsExitButton",
                         "Back",
                         "Return to main view")
    , hideHeaderButton("HeaderHideButton", "HideHeader", "Hide header bar")
    , titleButton("HeaderTitleButton", "None", "Made by Dimethoxy")
    , updateButton("UpdateButton",
                   "Download",
                   "Update available - click to install")
    , bypassButton("BypassButton",
                   "Bypass",
                   "GlobalBypass",
                   _apvts,
                   "Toggle bypass")
    , presetsButton("PresetsButton", "Presets", "Browse presets")
    // Add new buttons for settings view
    , resetButton("HeaderResetButton", "Reload", "Reset settings")
    , saveButton("HeaderSaveButton", "Save", "Save settings")
  {
    // Shadows
    addAndMakeVisible(outerShadow);
    addAndMakeVisible(innerShadow);

    // Title
    addAndMakeVisible(title);
    addAndMakeVisible(titleButton);
    title.setText(_titleText);
    titleButton.setEnabled(false);

    // Settings Button
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(settingsButton);
    addAndMakeVisible(settingsExitButton);
    settingsExitButton.setVisible(false);
    addAndMakeVisible(hideHeaderButton);

    // These buttons are not done yet so we hide them for now
    addAndMakeVisible(updateButton);
    updateButton.setVisible(false);
    addAndMakeVisible(presetsButton);
    presetsButton.setVisible(false);

    // Add new buttons for settings view, hidden by default
    addAndMakeVisible(resetButton);
    resetButton.setVisible(false);
    addAndMakeVisible(saveButton);
    saveButton.setVisible(false);
  }

  //==============================================================================
  /** @brief Destructor for `Header`. */
  ~Header() noexcept override = default;

  //==============================================================================
  /**
   * @brief Paints the header component.
   *
   * This method draws the border and background of the header.
   *
   * @param _g The graphics context used for painting.
   */
  void paint(juce::Graphics& _g) noexcept override
  {
    // Paint the border
    auto bounds = getLocalBounds().removeFromTop(getHeight() * 0.5);
    _g.setColour(borderColor);
    _g.fillRect(bounds);

    // Paint the background
    const auto borderStrength = rawBorderStrength * size;
    bounds.removeFromBottom(static_cast<int>(borderStrength));
    _g.setColour(backgroundColour);
    _g.fillRect(bounds);
  }

  //==============================================================================
  /**
   * @brief Resizes the header and its child components.
   *
   * Dynamically adjusts the layout of the title, buttons, and shadows
   * based on the current size of the header.
   */
  void resized() noexcept override
  {
    auto bounds = getLocalBounds().removeFromTop(getHeight() * 0.5);
    const auto titleOffset = rawTitleOffset * size;
    const auto titleButtonWidth = rawTitleButtonWidth * size;
    const auto titleBounds =
      bounds.withWidth(static_cast<int>(titleButtonWidth))
        .withCentre(bounds.getCentre())
        .withY(static_cast<int>(titleOffset));
    title.setBounds(titleBounds);
    title.setAlwaysOnTop(true);

    // Set the bounds for the outer shadow
    juce::Path outerShadowPath;
    outerShadowPath.addRectangle(bounds);
    outerShadow.setPath(outerShadowPath);
    outerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);

    // Set the bounds for the inner shadow
    juce::Path innerShadowPath;
    innerShadowPath.addRectangle(bounds);
    innerShadow.setPath(innerShadowPath);
    innerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);

    // Set the bounds for the inner header
    const auto borderStrength = rawBorderStrength * size;
    bounds.removeFromBottom(static_cast<int>(borderStrength));

    // Header Button Bounds
    const int headerButtonAmount = 2;
    const auto headerButtonAreaWidth =
      rawHeaderButtonWidth * size * headerButtonAmount;
    const auto buttonAreaPadding = 2.0 * rawButtonPadding * size;
    auto buttonAreaBounds =
      bounds.withWidth(bounds.getWidth() - static_cast<int>(buttonAreaPadding))
        .withCentre(bounds.getCentre());

    // Set the bounds for the settings button
    const auto settingsButtonBounds = buttonAreaBounds.removeFromRight(
      static_cast<int>(rawHeaderButtonWidth * size));
    settingsButton.setBounds(settingsButtonBounds);
    settingsExitButton.setBounds(settingsButtonBounds);

    // Set the bounds for the hide header button
    const auto hideHeaderButtonBounds = buttonAreaBounds.removeFromRight(
      static_cast<int>(rawHeaderButtonWidth * size));
    hideHeaderButton.setBounds(hideHeaderButtonBounds);

    // Set the bounds for the update download button
    const auto updateButtonBounds = buttonAreaBounds.removeFromRight(
      static_cast<int>(rawHeaderButtonWidth * size));
    updateButton.setBounds(updateButtonBounds);

    // Set the bounds for the reset and save buttons (settings view only)
    resetButton.setBounds(updateButtonBounds);    // reset replaces update
    saveButton.setBounds(hideHeaderButtonBounds); // save replaces hide header

    // Set the bounds for the bypass button
    const auto bypassButtonBounds = buttonAreaBounds.removeFromLeft(
      static_cast<int>(rawHeaderButtonWidth * size));
    bypassButton.setBounds(bypassButtonBounds);

    // Set the bounds for the presets button
    const auto presetsButtonBounds = buttonAreaBounds.removeFromLeft(
      static_cast<int>(rawHeaderButtonWidth * size));
    presetsButton.setBounds(presetsButtonBounds);

    // Set the bounds for the title button
    const auto titleButtonBounds =
      bounds.withWidth(static_cast<int>(titleButtonWidth))
        .withCentre(bounds.getCentre());
    titleButton.setBounds(titleButtonBounds);
  }

  //==============================================================================
  /** @brief Returns the settings button. */
  [[nodiscard]] CallbackButton& getSettingsButton() noexcept
  {
    return settingsButton;
  }

  /** @brief Returns the settings exit button. */
  [[nodiscard]] CallbackButton& getSettingsExitButton() noexcept
  {
    return settingsExitButton;
  }

  /** @brief Returns the hide header button. */
  [[nodiscard]] CallbackButton& getHideHeaderButton() noexcept
  {
    return hideHeaderButton;
  }

  /** @brief Returns the update button. */
  [[nodiscard]] CallbackButton& getUpdateButton() noexcept
  {
    return updateButton;
  }

  /** @brief Returns the bypass button. */
  [[nodiscard]] ToggleButton& getBypassButton() noexcept
  {
    return bypassButton;
  }

  /** @bried Returns the save button. */
  [[nodiscard]] CallbackButton& getSaveButton() noexcept { return saveButton; }

  /** @brief Returns the reset button. */
  [[nodiscard]] CallbackButton& getResetButton() noexcept
  {
    return resetButton;
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  Shadow outerShadow;
  Shadow innerShadow;
  Fonts fonts; // <-- Moved this above Label title
  Label title;
  CallbackButton settingsButton;
  CallbackButton settingsExitButton;
  CallbackButton hideHeaderButton;
  CallbackButton titleButton;
  CallbackButton updateButton;
  ToggleButton bypassButton;
  CallbackButton presetsButton;
  CallbackButton resetButton;
  CallbackButton saveButton;

  //==============================================================================
  // Other members

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Header)
};

} // namespace window
} // namespace gui
} // namespace dmt