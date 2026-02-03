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
 * Provides a base class for components that require periodic repainting at a
 * configurable frame rate. Designed for real-time GUI responsiveness and
 * dynamic framerate adaptation, using JUCE's Timer system.
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

namespace dmt {
namespace utility {

//==============================================================================
/**
 * @brief Base class for repaint-timed components with dynamic framerate.
 *
 * @details
 * This class provides a robust, type-safe mechanism for triggering periodic
 * repaints in JUCE-based components. It automatically adapts to changes in
 * the global framerate setting, ensuring smooth GUI updates without manual
 * polling. Intended for real-time applications where repaint frequency may
 * change at runtime.
 *
 * @note
 * Subclasses must implement repaintTimerCallback(), which is called at the
 * configured framerate. The timer will automatically restart if the framerate
 * changes.
 *
 * @warning
 * This class is non-copyable and non-movable. Use only as a base class.
 */
//==============================================================================
class RepaintTimer : private juce::Timer
{
  //============================================================================
  // Settings
  const int& fps = Settings::framerate;

protected:
  //============================================================================
  /**
   * @brief Constructs a RepaintTimer.
   *
   * @details
   * The constructor is constexpr for maximum compile-time optimization and
   * compatibility with modern C++ standards. The timer is not started
   * automatically; call startRepaintTimer() to begin.
   */
  inline RepaintTimer() noexcept = default;

  //============================================================================
  /**
   * @brief Destructor.
   *
   * @details
   * Ensures that the timer is stopped and resources are released. No
   * side-effects beyond JUCE Timer cleanup.
   */
  inline ~RepaintTimer() noexcept override = default;

  //============================================================================
  /**
   * @brief Starts the repaint timer at the current global framerate.
   *
   * @details
   * If the timer is already running, this function is a no-op. Otherwise,
   * it initializes the timer with the current framerate from Settings.
   * The timer will automatically adapt if the framerate changes.
   *
   * @note
   * This method is noexcept and safe to call repeatedly.
   */
  inline void startRepaintTimer() noexcept
  {
    if (this->isTimerRunning())
      return;

    this->currentFps = fps;
    startTimerHz(fps);
  }

  //============================================================================
  /**
   * @brief Stops the repaint timer.
   *
   * @details
   * Halts all periodic repaint callbacks. Safe to call even if the timer
   * is not running.
   */
  inline void stopRepaintTimer() noexcept { stopTimer(); }

  //============================================================================
  /**
   * @brief Called periodically at the configured framerate.
   *
   * @details
   * Subclasses must implement this pure virtual function to perform
   * repaint logic. This is called from the timer thread and should be
   * real-time safe.
   */
  virtual void repaintTimerCallback() noexcept = 0;

private:
  //============================================================================
  /**
   * @brief Internal JUCE timer callback.
   *
   * @details
   * Invokes repaintTimerCallback() and checks for framerate changes.
   * If the global framerate has changed, the timer is restarted to
   * match the new rate. This ensures seamless adaptation to runtime
   * framerate adjustments.
   */
  inline void timerCallback() override
  {
    repaintTimerCallback();
    if (fps != currentFps) {
      currentFps = fps;
      stopRepaintTimer();
      startRepaintTimer();
    }
  }

  //============================================================================
  // Other members
  int currentFps{};

  //============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RepaintTimer)
};
//==============================================================================
} // namespace utility
} // namespace dmt
