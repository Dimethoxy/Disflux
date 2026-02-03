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
 * Get the options for the properties file with predefined settings.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace configuration {

//==============================================================================
/**
 * @brief Get the options for the properties file with predefined settings.
 *        Set's the application name, folder name, and other options.
 *
 * @return A juce::PropertiesFile::Options object with default settings.
 */
[[nodiscard]] static inline auto
getOptions() noexcept
{
  juce::PropertiesFile::Options options;

  constexpr auto& name = ProjectInfo::projectName;
  options.applicationName = name;

  options.filenameSuffix = ".config";
  options.storageFormat = juce::PropertiesFile::storeAsXML;

  if constexpr (OS_IS_WINDOWS) {
    options.folderName = juce::String("Dimethoxy/") + name;
  } else if constexpr (OS_IS_DARWIN) {
    options.folderName = juce::String("Dimethoxy/") + name;
  } else if constexpr (OS_IS_LINUX) {
    options.folderName = juce::String(".config/Dimethoxy/") + name;
  }

  options.osxLibrarySubFolder = "Application Support";
  options.commonToAllUsers = false;
  options.ignoreCaseOfKeyNames = true;
  options.doNotSave = false;
  options.millisecondsBeforeSaving = -1;

  return options;
}

//==============================================================================

} // namespace configuration
} // namespace dmt

//==============================================================================