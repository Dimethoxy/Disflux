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
 * ToggleButton class for linking GUI button state to an
 * AudioProcessorValueTreeState parameter, with real-time state synchronization
 * and visual feedback.
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
 *   A toggle button that synchronizes its state with an
 * AudioProcessorValueTreeState parameter.
 *
 * @details
 *   This button listens to a parameter in the APVTS and updates its visual
 * state accordingly. Clicking the button toggles the parameter value, and any
 * external parameter changes are reflected in the button's appearance. Designed
 * for real-time plugin GUIs where parameter and UI state must remain in sync.
 */
class ToggleButton
  : public dmt::gui::widget::AbstractButton
  , public juce::AudioProcessorValueTreeState::Listener
{
  //==============================================================================
  using AbstractButton = dmt::gui::widget::AbstractButton;

public:
  //==============================================================================
  /**
   * @brief
   *   Constructs a ToggleButton linked to a parameter in the APVTS.
   *
   * @param _name The button's name.
   * @param _iconName The icon name for the button.
   * @param _parameterID The parameter ID in the APVTS to control.
   * @param _apvts Reference to the APVTS instance.
   * @param _tooltip The tooltip text for the button.
   * @param _shouldDrawBorder Whether to draw the button's border.
   * @param _shouldDrawBackground Whether to draw the button's background.
   * @param _shouldDrawShadow Whether to draw the button's shadow.
   * @param _alternativeIconHover Whether to use an alternative icon on hover.
   *
   * @details
   *   The constructor registers the button as a listener to the parameter,
   *   and sets the initial visual state based on the parameter's value.
   */
  explicit ToggleButton(juce::String _name,
                        juce::String _iconName,
                        juce::String _parameterID,
                        AudioProcessorValueTreeState& _apvts,
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
    , parameterID(_parameterID)
    , apvts(_apvts)
  {
    TRACER("ToggleButton::ToggleButton");
    // Start listening to the parameter
    apvts.addParameterListener(parameterID, this);

    // Set the button's initial state based on the parameter value
    auto* bypassParameter = apvts.getParameter(parameterID);
    if (bypassParameter != nullptr) {
      auto bypassValue = bypassParameter->getValue();
      if (bypassValue == 0.0f) {
        setPassiveState();
      } else {
        setClickedState();
      }
    } else {
      jassertfalse; // Parameter not found
    }
  }

  //==============================================================================
  /**
   * @brief
   *   Destructor. Removes the parameter listener.
   */
  inline ~ToggleButton() override
  {
    TRACER("ToggleButton::~ToggleButton");
    apvts.removeParameterListener(parameterID, this);
  }

  //==============================================================================
  /**
   * @brief
   *   Handles mouse enter events to update the button's visual state.
   *
   * @param _event The mouse event(unused).
   *
   *
   * @details *If the button is enabled and the parameter is off, transitions to
   * hover state.
   */
  inline void mouseEnter(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("ToggleButton::mouseEnter");
    if (isEnabled()) {
      auto* bypassParameter = apvts.getParameter(parameterID);
      if (bypassParameter != nullptr && bypassParameter->getValue() == 0.0f) {
        setHoverState();
      }
    }
  }

  //==============================================================================
  /**
   * @brief
   *   Handles mouse exit events to revert the button's visual state.
   *
   * @param _event The mouse event(unused).
   *
   * @details If the button is enabled and the parameter is off, returns to
   * passive state.
   */
  inline void mouseExit(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("ToggleButton::mouseExit");
    if (isEnabled()) {
      auto* bypassParameter = apvts.getParameter(parameterID);
      if (bypassParameter != nullptr && bypassParameter->getValue() == 0.0f) {
        setPassiveState();
      }
    }
  }

  //==============================================================================
  /**
   * @brief
   *   Handles mouse down events. No visual state change for toggle.
   *
   * @param _event The mouse event (unused).
   */
  inline void mouseDown(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("ToggleButton::mouseDown");
    if (isEnabled()) {
      // No visual state change on mouse down for toggle.
    }
  }

  //==============================================================================
  /**
   * @brief
   *   Handles mouse up events to toggle the parameter value.
   *
   * @param _event The mouse event (unused).
   *
   * @details
   *   If enabled, toggles the parameter value and notifies the host.
   */
  inline void mouseUp(const juce::MouseEvent& /*_event*/) override
  {
    TRACER("ToggleButton::mouseUp");
    if (isEnabled()) {
      // Toggle the parameter value
      auto* bypassParameter = apvts.getParameter(parameterID);
      if (bypassParameter != nullptr) {
        auto newValue = bypassParameter->getValue() == 0.0f ? 1.0f : 0.0f;
        bypassParameter->setValueNotifyingHost(newValue);
      }
    }
  }

  //==============================================================================
  /**
   * @brief
   *   Called when the linked parameter changes.
   *
   * @param _parameterID The parameter ID that changed.
   * @param _newValue The new value of the parameter.
   *
   * @details
   *   Updates the button's visual state to match the parameter.
   */
  inline void parameterChanged(const juce::String& _parameterID,
                               float _newValue) override
  {
    TRACER("ToggleButton::parameterChanged");
    if (_parameterID == parameterID) {
      if (_newValue == 0.0f) {
        setPassiveState();
      } else {
        setClickedState();
      }
    }
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  juce::String parameterID;
  AudioProcessorValueTreeState& apvts;

  //==============================================================================
  // Other members
  // (none)

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleButton)
};

} // namespace widget
} // namespace gui
} // namespace dmt