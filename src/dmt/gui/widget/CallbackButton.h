//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ██   ███████ ██    ██   ███     ████
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
 * Implementation AbstractButton class designed to trigger callbacks on click.
 * It supports hover and click states, customizable colors, and tooltips.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/widget/AbstractButton.h"
#include "gui/widget/Shadow.h"
#include "utility/Icon.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================
/**
 * @brief
 *  A callback-enabled button with customizable icon, tooltip, and visual
 *  states.
 *
 * @details
 *  This class extends AbstractButton to provide a button that triggers a
 *  callback when clicked. It supports hover and click states, customizable
 *  colors, and tooltips. The button's appearance is optimized for real-time
 *  GUI performance.
 *
 * @note
 *  The button's visual states are managed internally, and the constructor
 *  initializes the button with a passive state for consistent appearance.
 */
class CallbackButton final : public dmt::gui::widget::AbstractButton
{
  //==============================================================================
  using AbstractButton = dmt::gui::widget::AbstractButton;

public:
  //==============================================================================
  /**
   * @brief
   *   Constructs a CallbackButton with the specified parameters.
   *
   * @param _name The button's name (used for accessibility and identification).
   * @param _iconName The icon to display on the button.
   * @param _tooltip The tooltip text to show on hover (optional).
   * @param _shouldDrawBorder Whether to draw a border around the button.
   * @param _shouldDrawBackground Whether to draw a background behind the icon.
   * @param _shouldDrawShadow Whether to draw a drop shadow.
   * @param _alternativeIconHover Whether to use an alternative icon on hover.
   *
   * @details
   *   The constructor initializes the button and sets its initial state to
   *   passive for visual consistency. All visual options are configurable.
   */
  explicit CallbackButton(juce::String _name,
                          juce::String _iconName,
                          juce::String _tooltip = "",
                          bool _shouldDrawBorder = true,
                          bool _shouldDrawBackground = true,
                          bool _shouldDrawShadow = true,
                          bool _alternativeIconHover = false) noexcept
    : AbstractButton(_name,
                     _iconName,
                     _tooltip,
                     _shouldDrawBorder,
                     _shouldDrawBackground,
                     _shouldDrawShadow,
                     _alternativeIconHover)
  {
    TRACER("CallbackButton::CallbackButton");
    setPassiveState(); // Set initial state for consistent appearance
  }

  //==============================================================================
  /**
   * @brief
   *   Destructor. No special cleanup required.
   */
  ~CallbackButton() override = default;

  //==============================================================================
  /**
   * @brief
   *   Handles mouse enter events to update the button's visual state.
   *
   * @param _event The mouse event (unused).
   *
   * @details
   *   When the mouse enters the button and it is enabled, the button
   * transitions to the hover state for immediate visual feedback.
   */
  inline void mouseEnter(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("CallbackButton::mouseEnter");
    if (isEnabled()) {
      setHoverState();
    }
  }

  //==============================================================================
  /**
   * @brief
   *   Handles mouse exit events to revert the button's visual state.
   *
   * @param _event The mouse event (unused).
   *
   * @details
   *   When the mouse leaves the button and it is enabled, the button returns to
   *   the passive state to indicate it is no longer hovered.
   */
  inline void mouseExit(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("CallbackButton::mouseExit");
    if (isEnabled()) {
      setPassiveState();
    }
  }

  //==============================================================================
  /**
   * @brief
   *   Handles mouse down events to indicate the button is being pressed.
   *
   * @param _event The mouse event (unused).
   *
   * @details
   *   When the button is pressed and enabled, it transitions to the clicked
   *   state for immediate visual feedback.
   */
  inline void mouseDown(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("CallbackButton::mouseDown");
    if (isEnabled()) {
      setClickedState();
    }
  }

  //==============================================================================
  /**
   * @brief
   *   Handles mouse up events to trigger the button's callback and update
   * state.
   *
   * @param _event The mouse event (unused).
   *
   * @details
   *   On mouse release, if the button is enabled, it transitions to the hover
   *   state and triggers the onClick callback if set. This ensures the action
   * is only performed on mouse up, following standard UI conventions.
   */
  inline void mouseUp(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("CallbackButton::mouseUp");
    if (isEnabled()) {
      setHoverState();
      if (onClick)
        onClick(); // Trigger the button's click action if set
    }
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  // (none for this class)

  //==============================================================================
  // Other members
  // (none for this class)

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CallbackButton)
};

} // namespace widget
} // namespace gui
} // namespace dmt