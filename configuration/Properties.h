//==============================================================================
/*
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
 * Singleton class to manage application properties with optimized
 * performance.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>
#include <configuration/Container.h>
#include <configuration/Options.h>

//==============================================================================

namespace dmt {
namespace configuration {

//==============================================================================

using SettingValue = dmt::configuration::Container::SettingValue;
using SettingsOverwride = std::pair<String, SettingValue>;
using SettingsOverrides = std::vector<SettingsOverwride>;
using SettingsReplacement = std::pair<SettingValue, SettingValue>;
using SettingsReplacements = std::vector<SettingsReplacement>;
//==============================================================================
/**
 * @class Properties
 * @brief Class to manage application properties
 */
class Properties
{
  using String = juce::String;

public:
  //============================================================================
  /**
   * @brief Initialize the properties with options and settings.
   */
  void initialize(SettingsOverrides _overwrites = {},
                  SettingsReplacements _replacements = {}) noexcept
  {
    auto options = dmt::configuration::getOptions();
    file.setStorageParameters(options);
    auto settings = file.getUserSettings();

    // Build fallback property set from defaults
    fallbackPropertySet = dmt::Settings::container.toPropertySet();
    // Apply overrides and replacements to fallback only
    applyOverrides(&fallbackPropertySet, _overwrites);
    applyReplacements(&fallbackPropertySet, _replacements);
    settings->setFallbackPropertySet(&fallbackPropertySet);

    // Add missing keys from the fallback property set to user settings
    bool newKeysAdded = false;
    const auto& fallbackKeys = fallbackPropertySet.getAllProperties();
    for (const auto& key : fallbackKeys.getAllKeys()) {
      if (!settings->containsKey(key)) {
        settings->setValue(key, fallbackKeys[key]);
        newKeysAdded = true; // Mark that new keys were added
      }
    }

    // Remove the "initialized" flag if new keys were added
    if (newKeysAdded && settings->containsKey("initialized")) {
      settings->removeValue("initialized");
    }

    // Mark as initialized and save the settings
    if (!settings->containsKey("initialized")) {
      settings->setValue("initialized", true);
    }
    settings->saveIfNeeded();

    // Now we apply the settings to the container
    dmt::Settings::container.applyPropertySet(settings);

    // Set the app name
    auto appName = options.applicationName;
    dmt::Settings::appName = appName;
  }

  /**
   * @brief Save the current container settings to the file system.
   */
  void saveCurrentSettings()
  {
    auto* settings = file.getUserSettings();
    auto currentSet = dmt::Settings::container.toPropertySet();
    for (const auto& key : currentSet.getAllProperties().getAllKeys()) {
      settings->setValue(key, currentSet.getValue(key));
    }
    settings->saveIfNeeded();
  }

  /**
   * @brief Reset the container and file to fallback (default) values.
   */
  void resetToFallback()
  {
    auto* settings = file.getUserSettings();
    // Overwrite container with fallback
    dmt::Settings::container.applyPropertySet(&fallbackPropertySet);
    // Overwrite file with fallback
    for (const auto& key :
         fallbackPropertySet.getAllProperties().getAllKeys()) {
      settings->setValue(key, fallbackPropertySet.getValue(key));
    }
    settings->saveIfNeeded();
  }

protected:
  void applyOverrides(juce::PropertySet* settings,
                      const SettingsOverrides& _overwrites)
  {
    for (const auto& [key, value] : _overwrites) {
      if (std::holds_alternative<String>(value)) {
        settings->setValue(key, std::get<String>(value));
      } else if (std::holds_alternative<int>(value)) {
        settings->setValue(key, std::get<int>(value));
      } else if (std::holds_alternative<float>(value)) {
        settings->setValue(key, std::get<float>(value));
      } else if (std::holds_alternative<bool>(value)) {
        settings->setValue(key, std::get<bool>(value));
      } else if (std::holds_alternative<juce::Colour>(value)) {
        settings->setValue(key, std::get<juce::Colour>(value).toString());
      }
    }
  }

  void applyReplacements(juce::PropertySet* settings,
                         const SettingsReplacements& _replacements)
  {
    for (const auto& [fromValue, toValue] : _replacements) {
      auto allKeys = settings->getAllProperties().getAllKeys();
      for (const auto& key : allKeys) {
        const auto& currentValue = settings->getValue(key);

        // Compare and replace for each supported type
        bool match = false;
        juce::var newValue;
        if (std::holds_alternative<String>(fromValue)) {
          if (currentValue == std::get<String>(fromValue))
            match = true;
        } else if (std::holds_alternative<int>(fromValue)) {
          if (currentValue.getIntValue() == std::get<int>(fromValue))
            match = true;
        } else if (std::holds_alternative<float>(fromValue)) {
          if (currentValue.getFloatValue() == std::get<float>(fromValue))
            match = true;
        } else if (std::holds_alternative<bool>(fromValue)) {
          bool currentBool = (currentValue == "1" || currentValue == "true");
          if (currentBool == std::get<bool>(fromValue))
            match = true;
        } else if (std::holds_alternative<juce::Colour>(fromValue)) {
          if (juce::Colour::fromString(currentValue) ==
              std::get<juce::Colour>(fromValue))
            match = true;
        }

        if (match) {
          // Set toValue as the new value
          if (std::holds_alternative<String>(toValue)) {
            newValue = std::get<String>(toValue);
          } else if (std::holds_alternative<int>(toValue)) {
            newValue = std::get<int>(toValue);
          } else if (std::holds_alternative<float>(toValue)) {
            newValue = std::get<float>(toValue);
          } else if (std::holds_alternative<bool>(toValue)) {
            newValue = std::get<bool>(toValue);
          } else if (std::holds_alternative<juce::Colour>(toValue)) {
            newValue = std::get<juce::Colour>(toValue).toString();
          }
          settings->setValue(key, newValue);
        }
      }
    }
  }

private:
  juce::ApplicationProperties file;
  juce::PropertySet fallbackPropertySet;
};
} // namespace configuration
} // namespace dmt