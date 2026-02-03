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
 * Type-safe settings container for Dimethoxy Audio applications.
 * Provides runtime-checked, variant-based storage for configuration values.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>
#include <any>

//==============================================================================

namespace dmt {
namespace configuration {

//==============================================================================
/**
 * @brief A type-safe container for application settings.
 *
 * @details
 * This class allows adding and retrieving settings with different types.
 * It ensures that the types of the settings are consistent, and provides
 * type-safe access to the values. All access is checked at runtime.
 */
class Container
{
  //============================================================================
  // Aliases for convenience
  using String = juce::String;
  using Colour = juce::Colour;

public:
  //============================================================================
  /**
   * @brief Pseudo type for storing heterogeneous settings.
   *
   * @details
   * The possible types are: String, Colour, int, float, and bool.
   * Used for type-safe, runtime-checked configuration storage.
   */
  typedef std::variant<String, Colour, int, float, bool> SettingValue;

  //============================================================================
  /**
   * @brief Type for storing a range limit.
   *
   * @details
   * Used for storing the range limits of settings.
   * The range is inclusive.
   */
  template<typename T>
  struct Range
  {
    T min;
    T max;

    Range(T _min, T _max)
      : min(_min)
      , max(_max)
    {
    }
  };

  //============================================================================
  /**
   * @brief Default constructor.
   */
  Container() = default;

  //============================================================================
  /**
   * @brief Retrieves a setting by its name with type safety.
   *
   * @tparam T The type of the setting to retrieve.
   * @param _name The name of the setting.
   * @return A mutable reference to the setting value of type T.
   *
   * @throws std::runtime_error If the setting is not found or the type does not
   * match.
   *
   * @details
   * Throws if the type does not match or the setting does not exist.
   * Use for real-time safe, type-checked access to configuration.
   */
  template<typename T>
  inline T& get(const String _name)
  {
    auto it = settings.find(_name);
    if (it != settings.end()) {
      if (std::holds_alternative<T>(it->second)) {
        return std::get<T>(it->second);
      } else {
        jassertfalse;
        throw std::runtime_error("Type mismatch for setting: " +
                                 _name.toStdString());
      }
    } else {
      jassertfalse;
      throw std::runtime_error("Setting not found: " + _name.toStdString());
    }
  }

  //==============================================================================
  /**
   * @brief Adds a new setting or ensures consistency for an existing one.
   *
   * @tparam T The type of the setting to add.
   * @param _name The name of the setting.
   * @param _value The value to associate with the setting.
   * @return A mutable reference to the setting value of type T.
   *
   * @throws std::runtime_error If there is a type mismatch when adding an
   * existing setting.
   *
   * @details
   * If the setting already exists, the type must match the stored type.
   * If it doesn't match, an error is thrown. If the setting doesn't exist,
   * it is added to the collection.
   */
  template<typename T>
  inline T& add(const String _name,
                const T _value,
                const T* min = nullptr,
                const T* max = nullptr)
  {
    auto it = settings.find(_name);
    if (it != settings.end()) {
      if (!std::holds_alternative<T>(it->second)) {
        jassertfalse;
        throw std::runtime_error("Type mismatch for setting: " +
                                 _name.toStdString());
      }
    } else {
      settings[_name] = _value;
      if (min && max) {
        ranges[_name] = Range<T>(*min, *max);
      }
    }
    return std::get<T>(settings[_name]);
  }

  //==============================================================================
  /**
   * @brief Converts all settings to a juce::PropertySet.
   *
   * @return A juce::PropertySet containing all settings as string values.
   *
   * @details
   * Used for serialization or export of settings to JUCE property sets.
   */
  inline juce::PropertySet toPropertySet() const
  {
    juce::PropertySet propertySet;
    for (const auto& [key, value] : settings) {
      if (std::holds_alternative<String>(value)) {
        propertySet.setValue(key, std::get<String>(value));
      } else if (std::holds_alternative<Colour>(value)) {
        propertySet.setValue(key, std::get<Colour>(value).toString());
      } else if (std::holds_alternative<int>(value)) {
        propertySet.setValue(key, std::get<int>(value));
      } else if (std::holds_alternative<float>(value)) {
        propertySet.setValue(key, std::get<float>(value));
      } else if (std::holds_alternative<bool>(value)) {
        propertySet.setValue(key, std::get<bool>(value));
      }
    }
    return propertySet;
  }

  //==============================================================================
  /**
   * @brief Applies a juce::PropertySet to the settings container.
   *
   * @param _propertySet Pointer to the property set to apply.
   *
   * @details
   * Updates all settings from the property set, converting types as needed.
   * Only updates settings that already exist in the container.
   */
  inline void applyPropertySet(juce::PropertySet* _propertySet)
  {
    for (auto& [key, storedValue] : settings) {
      if (_propertySet->containsKey(key)) {
        if (std::holds_alternative<String>(storedValue)) {
          settings[key] = _propertySet->getValue(key);
        } else if (std::holds_alternative<Colour>(storedValue)) {
          settings[key] = Colour::fromString(_propertySet->getValue(key));
        } else if (std::holds_alternative<int>(storedValue)) {
          settings[key] = _propertySet->getValue(key).getIntValue();
        } else if (std::holds_alternative<float>(storedValue)) {
          settings[key] = _propertySet->getValue(key).getFloatValue();
        } else if (std::holds_alternative<bool>(storedValue)) {
          settings[key] = _propertySet->getBoolValue(key);
        }
      }
    }
  }

  //==============================================================================
  /**
   * @brief Returns a mutable reference to the internal settings map.
   *
   * @details
   * This is used by adapters that need to provide mutable access to values.
   */
  inline std::map<String, SettingValue>& getAllSettingsMutable()
  {
    return settings;
  }

  //==============================================================================
  /**
   * @brief Tries to retrieve the range of a setting by its name.
   *
   */
  template<typename T>
  inline std::optional<Range<T>> getRange(const String _name)
  {
    auto pair = ranges.find(_name);
    if (pair != ranges.end()) {
      if (auto rangePtr = std::any_cast<Range<T>>(&pair->second)) {
        return *rangePtr;
      } else {
        jassertfalse;
        throw std::runtime_error("Type mismatch for range: " +
                                 _name.toStdString());
      }
    }
    return std::nullopt; // Return empty optional if range is not found
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  // (none for this class)

  //==============================================================================
  // Other members
  std::map<String, SettingValue> settings;
  std::map<String, std::any> ranges;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Container)
};
//==============================================================================
} // namespace configuration
} // namespace dmt