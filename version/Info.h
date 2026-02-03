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
 * Version information struct for Dimethoxy Audio applications.
 * Holds current, latest, and update-related version state.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace version {

//==============================================================================
/**
 * @brief Holds version and update state for the application.
 *
 * @details
 * This struct is used as a global singleton for version state, including
 * current version, latest version, update status, and download link.
 */
struct Info
{
  using VersionArray = std::array<int, 3>;

  static inline std::unique_ptr<VersionArray> current;
  static inline std::unique_ptr<VersionArray> latest;
  static inline std::unique_ptr<bool> isLatest;
  static inline std::unique_ptr<juce::String> downloadLink;
  static inline bool wasPopoverShown = false;
};
//==============================================================================
} // namespace version
} // namespace dmt