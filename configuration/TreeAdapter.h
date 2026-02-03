//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ███████ ██    ██   ███     ████
 * ██   ██ ██ ██  ██  ██ ██         ██   ██ ██    ██  ██ ██     ██
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
 * Adapter for browsing settings as a fixed-depth category/leaf tree.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "configuration/Container.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace configuration {

//==============================================================================
/**
 * @brief Adapter for browsing settings as a category/leaf tree.
 *
 * @details
 * This adapter provides a fixed-depth (category/leaf) view of a flat
 * dmt::configuration::Container. It is intended for use in GUIs or
 * other tools that require hierarchical browsing of settings.
 *
 * The adapter groups settings by splitting their names at the first '.'
 * character, treating the prefix as the category and the suffix as the leaf.
 * Only settings with at least one '.' are included in the tree.
 *
 * The adapter holds a reference to the underlying Container, so all
 * mutations are reflected in real time. Call rebuild() if settings are
 * added or removed after construction.
 */
class TreeAdapter
{
  using Container = dmt::configuration::Container;

public:
  //==============================================================================
  /**
   * @brief Represents a leaf node in the settings tree.
   *
   * @details
   * Each leaf corresponds to a single setting value within a category.
   * The value pointer is always valid as long as the Container exists.
   */
  struct Leaf
  {
    juce::String name;
    Container::SettingValue* value;
    std::shared_ptr<void> range; // Pointer to range, nullptr if not found

    juce::String toString() const
    {
      auto toStringImpl = [](const auto& v) -> juce::String {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, juce::String>)
          return v;
        else if constexpr (std::is_same_v<T, juce::Colour>)
          return v.toString();
        else if constexpr (std::is_same_v<T, int>)
          return juce::String(v);
        else if constexpr (std::is_same_v<T, float>)
          return juce::String(v);
        else if constexpr (std::is_same_v<T, bool>)
          return v ? "true" : "false";
        else
          return {};
      };
      return value ? std::visit(toStringImpl, *value) : juce::String();
    }

    /**
     * @brief Returns the std::variant index of the value, or -1 if value is
     * null.
     */
    int getTypeIndex() const noexcept
    {
      return value ? static_cast<int>(value->index()) : -1;
    }

    /**
     * @brief Attempts to parse the given text and set the value.
     *
     * @param _textToSet The text to parse and set.
     * @return true if parsing and setting succeeded, false otherwise.
     */
    bool parseAndSet(juce::String _textToSet)
    {
      if (!value)
        return false;

      switch (getTypeIndex()) {
        case 0: // juce::String
          *value = _textToSet;
          return true;
        case 1: // juce::Colour
        {
          juce::Colour c = juce::Colour::fromString(_textToSet);
          if (c.isTransparent()) // fromString returns transparent if invalid
            return false;
          *value = c;
          return true;
        }
        case 2: // int
        {
          int v = _textToSet.getIntValue();
          // Optionally, check if _textToSet is a valid int string
          if (_textToSet.trim().isEmpty() ||
              !_textToSet.containsOnly("0123456789-+"))
            return false;
          // Check for range
          if (range != nullptr) {
            auto* rangePtr = static_cast<Container::Range<int>*>(range.get());
            if (v < rangePtr->min || v > rangePtr->max)
              return false;
          }
          *value = v;
          return true;
        }
        case 3: // float
        {
          double v = _textToSet.getDoubleValue();
          // Optionally, check if _textToSet is a valid float string
          if (_textToSet.trim().isEmpty())
            return false;
          // Check for range
          if (range != nullptr) {
            auto* rangePtr = static_cast<Container::Range<float>*>(range.get());
            if (v < rangePtr->min || v > rangePtr->max)
              return false;
          }
          *value = static_cast<float>(v);
          return true;
        }
        case 4: // bool
        {
          auto lower = _textToSet.trim().toLowerCase();
          if (lower == "true" || lower == "1") {
            *value = true;
            return true;
          }
          if (lower == "false" || lower == "0") {
            *value = false;
            return true;
          }
          return false;
        }
        default:
          return false;
      }
    }
  };

  //==============================================================================
  /**
   * @brief Represents a category node in the settings tree.
   *
   * @details
   * Each category contains a list of leaves (settings) that share the same
   * category prefix.
   */
  struct Category
  {
    juce::String name;
    std::vector<Leaf> leaves;
  };

  //==============================================================================
  /**
   * @brief Constructs the adapter with a reference to a Container.
   *
   * @param _containerRef Reference to the settings container to adapt.
   *
   * @details
   * The adapter does not take ownership. All mutations to the container
   * are reflected in the adapter. Call rebuild() if the set of settings
   * changes after construction.
   */
  inline explicit TreeAdapter(Container& _containerRef) noexcept
    : container(_containerRef)
  {
    buildTree();
  }

  /**
   * @brief Constructs the adapter with a reference to a Container and a block
   * list.
   *
   * @param _containerRef Reference to the settings container to adapt.
   * @param _blockedCategories List of category names to block (ignore).
   */
  inline TreeAdapter(Container& _containerRef,
                     std::vector<juce::String> _blockedCategories) noexcept
    : container(_containerRef)
    , blockedCategories(std::move(_blockedCategories))
  {
    buildTree();
  }

  //==============================================================================
  /**
   * @brief Rebuilds the category/leaf tree from the container.
   *
   * @details
   * Call this after adding or removing settings in the container.
   * This operation is not real-time safe and should not be called from
   * the audio thread.
   */
  inline void rebuild() noexcept { buildTree(); }

  //==============================================================================
  /**
   * @brief Returns a mutable reference to the vector of categories.
   *
   * @return Reference to the vector of categories.
   *
   * @details
   * This allows modifications to the categories directly. Use with caution
   * as it bypasses the rebuild mechanism.
   */
  [[nodiscard]] inline std::vector<Category>& getCategories() noexcept
  {
    return categories;
  }

protected:
  //==============================================================================
  /**
   * @brief Builds the category/leaf tree from the container.
   *
   * @details
   * Groups all settings by their category prefix (before the first '.').
   * Only settings with a '.' in their name are included.
   * Categories in the block list are ignored.
   */
  inline void buildTree() noexcept
  {
    categories.clear();
    std::map<juce::String, std::vector<Leaf>> categoryMap;

    for (auto& [key, value] : container.getAllSettingsMutable()) {
      auto dotIndex = static_cast<int>(key.indexOfChar('.'));
      if (dotIndex < 0)
        continue;
      juce::String category = key.substring(0, dotIndex);
      // Blocked category check
      if (std::find(blockedCategories.begin(),
                    blockedCategories.end(),
                    category) != blockedCategories.end())
        continue;
      juce::String leaf = key.substring(dotIndex + 1);
      // Hide "General.ThemeVersion"
      if (category == "General" && leaf == "ThemeVersion")
        continue;

      // Find range pointer (nullptr if not found)
      std::shared_ptr<void> rangePtr = nullptr;
      int typeIndex = static_cast<int>(value.index());
      try {
        switch (typeIndex) {
          case 2: { // int
            auto opt = container.getRange<int>(key);
            if (opt)
              rangePtr = std::make_shared<Container::Range<int>>(*opt);
            break;
          }
          case 3: { // float
            auto opt = container.getRange<float>(key);
            if (opt)
              rangePtr = std::make_shared<Container::Range<float>>(*opt);
            break;
          }
          default:
            break;
        }
      } catch (...) {
        // ignore errors, leave rangePtr as nullptr
      }

      Leaf leafObj{ leaf, &value, rangePtr };
      categoryMap[category].push_back(leafObj);
    }

    // Prepare to sort categories: "General" should be first, "Audio" second
    std::vector<std::pair<juce::String, std::vector<Leaf>>> sortedCategories;
    auto generalIt = categoryMap.find("General");
    if (generalIt != categoryMap.end()) {
      // Add "General" first if it exists
      sortedCategories.push_back(*generalIt);
      categoryMap.erase(generalIt);
    }
    auto audioIt = categoryMap.find("Audio");
    if (audioIt != categoryMap.end()) {
      // Add "Audio" second if it exists
      sortedCategories.push_back(*audioIt);
      categoryMap.erase(audioIt);
    }
    // Add the rest
    for (auto& entry : categoryMap) {
      sortedCategories.push_back(entry);
    }

    // Now add to categories vector
    for (auto& [category, leaves] : sortedCategories) {
      categories.push_back(Category{ category, leaves });
    }
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  Container& container;
  std::vector<juce::String> blockedCategories; // Blocked category names

  //==============================================================================
  // Other members
  std::vector<Category> categories;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeAdapter)
};
} // namespace configuration
} // namespace dmt