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
 * Utility functions for version parsing, comparison, and conversion.
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
 * @brief Utility functions for version parsing and comparison.
 *
 * @details
 * Provides helpers for parsing version strings, and comparing versions.
 * Used throughout the version management subsystem.
 */
struct Utility
{
  //==============================================================================
  // Aliases for convenience
  using VersionArray = std::array<int, 3>;
  using String = juce::String;

  //==============================================================================
  /**
   * @brief Parses a version string from a server response.
   *
   * @param _response The raw server response containing a version field.
   * @return The extracted version string.
   *
   * @details
   * Assumes the response contains a "version" field in JSON format.
   * Extracts the version substring for further processing.
   */
  static inline String parseResponseToVersionString(
    const String& _response) noexcept
  {
    int start = _response.indexOf("version") + 8;
    auto secondHalf = _response.substring(start);
    auto tokens = juce::StringArray::fromTokens(secondHalf, "\"", "");
    auto versionString = tokens[1];
    std::cout << "Version String: " << versionString << std::endl;
    return versionString;
  }

  //==============================================================================
  /**
   * @brief Converts a version string to a VersionArray.
   *
   * @param _versionString The version string (e.g. "1.2.3").
   * @return A VersionArray with major, minor, and patch as integers.
   *
   * @details
   * Splits the version string by '.' and converts each part to an integer.
   * Used for type-safe version comparison.
   */
  static inline VersionArray parseVersionStringToArray(
    const String& _versionString) noexcept
  {
    auto versionArray = juce::StringArray::fromTokens(_versionString, ".", "");
    VersionArray result;
    for (int i = 0; i < 3; i++) {
      result[static_cast<std::size_t>(i)] = versionArray[i].getIntValue();
    }
    std::cout << "- Major:" << result[0] << std::endl;
    std::cout << "- Minor:" << result[1] << std::endl;
    std::cout << "- Patch:" << result[2] << std::endl;
    return result;
  }

  //==============================================================================
  /**
   * @brief Compares two version arrays.
   *
   * @param _version1 The first version array.
   * @param _version2 The second version array.
   * @return 0 if equal, 1 if _version1 > _version2, 2 if _version2 > _version1.
   *
   * @details
   * Compares each element (major, minor, patch) in order. Returns as soon as
   * a difference is found. Used for update logic and version checks.
   */
  static inline int compareVersions(const VersionArray& _version1,
                                    const VersionArray& _version2)
  {
    for (std::size_t i = 0; i < _version1.size(); ++i) {
      if (_version1[i] < _version2[i])
        return 2; // _version2 is greater than _version1
      else if (_version1[i] > _version2[i])
        return 1; // _version1 is greater than _version2
    }
    return 0; // Both versions are equal
  }
  //==============================================================================
};

} // namespace version
} // namespace dmt