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
 * This file contains the Resizable class and its helper interface. It provides
 * a mechanism for scaling GUI components in a platform- and DPI-aware way. The
 * class uses the Curiously Recurring Template Pattern (CRTP) to allow derived
 * classes to access their own implementation details, while also providing a
 * non-template RTTI interface (IScaleable) for dynamic discovery.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

// Forward declaration of Compositor to avoid circular dependency
namespace dmt {
namespace gui {
namespace window {
class Compositor;
}
}
}

//==============================================================================

namespace dmt {

//==============================================================================
/**
 * @brief Interface for scaleable GUI components.
 *
 * This non-template base interface enables runtime type identification (RTTI)
 * for all scaleable components, regardless of their template parameter.
 * It allows code (such as the Compositor) to dynamically discover and update
 * the scaling factor of any component in the hierarchy, without knowing the
 * exact derived type.
 *
 * @note All scaleable components must inherit from this interface, which is
 *       automatically handled by inheriting from dmt::Scaleable<T>.
 */
struct IScaleable
{
  virtual ~IScaleable() = default;

  /**
   * @brief Set the scaling factor for this component.
   * @param newSize The new scaling factor to apply.
   */
  virtual void setSizeFactor(const float& newSize) noexcept = 0;
};

//==============================================================================
/**
 * @brief CRTP base class for scaleable GUI components.
 *
 * This class provides a uniform interface and mechanism for scaling GUI
 * components in a platform- and DPI-aware way. It uses the Curiously Recurring
 * Template Pattern (CRTP) to allow derived classes to access their own
 * implementation details (such as getDesktopScaleFactor), while also providing
 * a non-template RTTI interface (IScaleable) for dynamic discovery.
 *
 * The `size` member is a reference to an internal float, which can be updated
 * at runtime via setSizeFactor(). This indirection allows the Compositor or
 * other parent components to propagate scaling changes throughout the entire
 * component tree, regardless of the actual derived type.
 *
 * @tparam Derived The derived component type (CRTP idiom).
 *
 * @note
 * - Always inherit from Scaleable<YourComponent> to participate in global
 *   scaling.
 * - Do not override setSizeFactor; use the provided mechanism.
 * - The friend declaration for Compositor allows it to access setSizeFactor
 *   for efficient recursive updates.
 * - This pattern is necessary because C++ templates do not support dynamic_cast
 *   across different instantiations, so we use IScaleable for RTTI and
 *   reinterpret_cast for safe access to the size member.
 */
template<typename Derived>
class Scaleable : public IScaleable
{
  friend class gui::window::Compositor;

public:
  //============================================================================
  /**
   * @brief Reference to the current scaling factor for this component.
   *
   * Use this in your drawing/layout code to scale dimensions appropriately.
   */
  const float& size = internalSize;

  /**
   * @brief Reference to the current platform DPI scaling factor.
   *
   * This is automatically calculated based on the OS and display.
   */
  const float scale;

  //============================================================================
  /**
   * @brief Default constructor.
   *
   * Initializes the scaling references.
   */
  Scaleable()
    : scale(getScaleFactor())
  {
  }

private:
  //============================================================================
  /**
   * @brief Helper to get the derived class pointer (CRTP).
   */
  inline Derived* getSelf() noexcept { return static_cast<Derived*>(this); }

  //============================================================================
  /**
   * @brief Get the platform DPI scaling factor.
   *
   * On macOS, multiplies by 2.0 for Retina displays.
   */
  const float getScaleFactor() noexcept
  {
    // Find the main display
    auto* mainDisplay =
      juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();

    if (mainDisplay == nullptr) {
      jassertfalse; // Could not find main display
      return 1.0f;
    }

    // Get the scale factor from the main display
    float tempScale = static_cast<float>(mainDisplay->scale);
    return tempScale;
  }

  /**
   * @brief Set the scaling factor for this component.
   *
   * This is called by the Compositor to propagate scaling changes.
   * Do not call directly it only exists for the Compositor to access.
   */
  void setSizeFactor(const float& newSize) noexcept override
  {
    if (&newSize != &internalSize) {
      internalSize = newSize;
    }
  }

private:
  float internalSize = 1.0f;

  //============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scaleable)
};
} // namespace dmt