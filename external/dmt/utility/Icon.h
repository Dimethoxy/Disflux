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
 * Provides icon retrieval and icon padding utilities for the UI. All icons are
 * loaded from embedded SVG binary data for real-time safety and deterministic
 * resource management. Intended for use in UI components requiring consistent
 * iconography and spacing.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "BinaryData.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace icons {

//==============================================================================
/**
 * @brief Retrieves a JUCE Drawable icon from embedded binary SVG data.
 *
 * @param _iconName The name of the icon to retrieve.
 * @return std::unique_ptr<juce::Drawable> The loaded Drawable, or nullptr if
 * not found.
 *
 * @details
 * This function maps a symbolic icon name to its corresponding SVG binary data,
 * returning a JUCE Drawable for UI rendering. All SVGs are embedded for
 * real-time safety. If the icon name is not recognized, returns nullptr.
 *
 * @note
 * The returned Drawable is heap-allocated and must be managed by the caller.
 */
//==============================================================================
[[nodiscard]] static inline std::unique_ptr<juce::Drawable>
getIcon(const juce::String _iconName) noexcept
{
  if (_iconName == "OscilloscopeZoom")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::speed_svg),
      static_cast<size_t>(BinaryData::speed_svgSize));
  if (_iconName == "OscilloscopeThickness")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::thickness_svg),
      static_cast<size_t>(BinaryData::thickness_svgSize));
  if (_iconName == "OscilloscopeGain")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::height_svg),
      static_cast<size_t>(BinaryData::height_svgSize));
  if (_iconName == "Settings")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::gear_svg),
      static_cast<size_t>(BinaryData::gear_svgSize));
  if (_iconName == "Back")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::back_svg),
      static_cast<size_t>(BinaryData::back_svgSize));
  if (_iconName == "HideHeader")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::angles_up_svg),
      static_cast<size_t>(BinaryData::angles_up_svgSize));
  if (_iconName == "Bypass")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::bypass_svg),
      static_cast<size_t>(BinaryData::bypass_svgSize));
  if (_iconName == "Download")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::download_svg),
      static_cast<size_t>(BinaryData::download_svgSize));
  if (_iconName == "Presets")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::presets_svg),
      static_cast<size_t>(BinaryData::presets_svgSize));
  if (_iconName == "Close")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::close_svg),
      static_cast<size_t>(BinaryData::close_svgSize));
  if (_iconName == "Save")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::save_svg),
      static_cast<size_t>(BinaryData::save_svgSize));
  if (_iconName == "Reload")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::reload_svg),
      static_cast<size_t>(BinaryData::reload_svgSize));
  if (_iconName == "Info")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::info_svg),
      static_cast<size_t>(BinaryData::info_svgSize));
  if (_iconName == "Warning")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::warning_svg),
      static_cast<size_t>(BinaryData::warning_svgSize));
  if (_iconName == "Error")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::error_svg),
      static_cast<size_t>(BinaryData::error_svgSize));
  if (_iconName == "Success")
    return juce::Drawable::createFromImageData(
      static_cast<const void*>(BinaryData::success_svg),
      static_cast<size_t>(BinaryData::success_svgSize));
  return nullptr;
};

//==============================================================================
/**
 * @brief Returns the recommended padding for a given icon.
 *
 * @param _iconName The name of the icon.
 * @return float The padding value in pixels.
 *
 * @details
 * This function provides per-icon padding recommendations for consistent
 * alignment and spacing in the UI. Padding values are empirically chosen
 * for each icon's visual geometry.
 *
 * @note
 * If the icon name is not recognized, returns 0.0f.
 */
//==============================================================================
[[nodiscard]] static inline float
getPadding(const juce::String& _iconName) noexcept
{
  if (_iconName == "OscilloscopeZoom")
    return 4.0f;
  if (_iconName == "OscilloscopeThickness")
    return 5.0f;
  if (_iconName == "OscilloscopeGain")
    return 3.5f;
  if (_iconName == "Settings")
    return 5.0f;
  if (_iconName == "Back")
    return 5.0f;
  if (_iconName == "HideHeader")
    return 5.0f;
  if (_iconName == "Bypass")
    return 5.0f;
  if (_iconName == "Download")
    return 5.0f;
  if (_iconName == "Presets")
    return 5.0f;
  if (_iconName == "Close")
    return 0.0f;
  if (_iconName == "Save")
    return 5.0f;
  if (_iconName == "Reload")
    return 4.0f;
  if (_iconName == "Info")
    return 0.0f;
  if (_iconName == "Warning")
    return 0.0f;
  if (_iconName == "Error")
    return 0.0f;
  if (_iconName == "Success")
    return 0.0f;
  return 0.0f;
};
//==============================================================================
} // namespace icons
} // namespace dmt