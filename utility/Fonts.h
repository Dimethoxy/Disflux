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
 * Provides a set of JUCE font objects and typefaces for consistent UI
 * typography. All fonts are loaded from embedded binary data for real-time
 * safety and deterministic resource management. Intended for use across all UI
 * components to ensure visual consistency and avoid runtime font loading.
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
namespace utility {

//==============================================================================
/**
 * @brief Holds preloaded JUCE font and typeface objects for UI consistency.
 *
 * @details
 * This struct provides access to a set of JUCE Font and Typeface objects,
 * each loaded from embedded binary font data. Fonts are constructed at
 * initialization to guarantee real-time safety and avoid runtime allocations.
 * Intended usage is to share a single Fonts instance across UI components.
 *
 * @note
 * Typeface pointers are managed by JUCE's reference counting. Fonts are
 * constructed using the corresponding typeface for deterministic rendering.
 *
 * @see juce::Font, juce::Typeface, BinaryData
 */
struct Fonts
{
  //==============================================================================
  using Typeface = juce::Typeface;
  using Font = juce::Font;

  //==============================================================================
  /**
   * @brief Constructs a Fonts object, preloading all typefaces and fonts.
   *
   * @details
   * All typefaces are loaded from embedded binary data at construction time.
   * This ensures that font resources are available for immediate use and
   * avoids any runtime file I/O or allocations during UI rendering.
   */
  inline explicit Fonts() noexcept = default;

  //==============================================================================
  /**
   * @brief Destructor for Fonts.
   *
   * @details
   * Relies on JUCE's reference counting for typeface cleanup. No manual
   * resource management is required.
   */
  inline ~Fonts() noexcept = default;

  //==============================================================================
  // Preloaded typefaces

  alignas(8) Typeface::Ptr displayTypeface = Typeface::createSystemTypefaceFor(
    static_cast<const void*>(BinaryData::SedgwickAveDisplayRegular_ttf),
    static_cast<size_t>(BinaryData::SedgwickAveDisplayRegular_ttfSize));

  alignas(8) Typeface::Ptr lightTypeface = Typeface::createSystemTypefaceFor(
    static_cast<const void*>(BinaryData::PoppinsLight_ttf),
    static_cast<size_t>(BinaryData::PoppinsLight_ttfSize));

  alignas(8) Typeface::Ptr regularTypeface = Typeface::createSystemTypefaceFor(
    static_cast<const void*>(BinaryData::PoppinsRegular_ttf),
    static_cast<size_t>(BinaryData::PoppinsRegular_ttfSize));

  alignas(8) Typeface::Ptr mediumTypeface = Typeface::createSystemTypefaceFor(
    static_cast<const void*>(BinaryData::PoppinsMedium_ttf),
    static_cast<size_t>(BinaryData::PoppinsMedium_ttfSize));

  alignas(8) Typeface::Ptr boldTypeface = Typeface::createSystemTypefaceFor(
    static_cast<const void*>(BinaryData::PoppinsBold_ttf),
    static_cast<size_t>(BinaryData::PoppinsBold_ttfSize));

  //==============================================================================
  // Preloaded fonts

  alignas(8) Font display = Font(FontOptions(displayTypeface));
  alignas(8) Font light = Font(FontOptions(lightTypeface));
  alignas(8) Font regular = Font(FontOptions(regularTypeface));
  alignas(8) Font medium = Font(FontOptions(mediumTypeface));
  alignas(8) Font bold = Font(FontOptions(boldTypeface));

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fonts)
};

} // namespace utility
} // namespace dmt
