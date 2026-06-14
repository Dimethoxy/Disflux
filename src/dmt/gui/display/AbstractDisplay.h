//==============================================================================
/* ██████╗ ██╗███╗   ███╗███████╗████████╗██╗  ██╗ ██████╗ ██╗  ██╗██╗   ██╗
 * ██╔══██╗██║████╗ ████║██╔════╝╚══██╔══╝██║  ██║██╔═══██╗╚██╗██╔╝╚██╗ ██╔╝
 * ██║  ██║██║██╔████╔██║█████╗     ██║   ███████║██║   ██║ ╚███╔╝  ╚████╔╝
 * ██║  ██║██║██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██║   ██║ ██╔██╗   ╚██╔╝
 * ██████╔╝██║██║ ╚═╝ ██║███████╗   ██║   ██║  ██║╚██████╔╝██╔╝ ██╗   ██║
 * ╚═════╝ ╚═╝╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * Part of the Dimethoxy Library, primarily intended for Dimethoxy plugins.
 * External use is permitted but not recommended.
 * No support or compatibility guarantees are provided.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * AbstractDisplay provides a base class for all display components
 * that require custom painting, shadow/border rendering, and repaint timing.
 * Designed for extensibility and real-time GUI performance.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/utility/Scaleable.h"
#include "gui/widget/Shadow.h"
#include "utility/RepaintTimer.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================
/**
 * @brief Abstract base class for display components with repaint timer.
 *
 * @details
 * This class provides a foundation for custom display components, handling
 * periodic repainting. Subclasses must implement resized and paint for their
 * own drawing and layout logic. Chrome (shadow/border) is handled by
 * MultiDisplay.
 */
class AbstractDisplay
  : public juce::Component
  , public dmt::utility::RepaintTimer
  , public dmt::Scaleable<AbstractDisplay>
{
  //============================================================================
  // Aliases for convenience
  using String = juce::String;

public:
  //==============================================================================
  /**
   * @brief Constructs an AbstractDisplay.
   *
   * @details
   * Starts the repaint timer. Subclasses should implement their own
   * layout and painting logic.
   */
  inline explicit AbstractDisplay(juce::String _displayName = "") noexcept
    : displayName(_displayName)
  {
    this->startRepaintTimer();
  }

  //============================================================================
  /** @brief Destructor for `AbstractDisplay`.
   */
  inline ~AbstractDisplay() { stopRepaintTimer(); }

  //==============================================================================
  /**
   * @brief Paints the component content.
   *
   * @param _g The graphics context.
   *
   * @details
   * Subclasses should override this method to draw their custom content.
   * Calls prepareNextFrame at the end of each paint.
   */
  inline void paint(juce::Graphics& _g) noexcept override
  {
    // Prepare next frame
    prepareNextFrame();
  }

  //==============================================================================
  /**
   * @brief Handles resizing.
   *
   * @details
   * Subclasses should override this method to layout their subcomponents.
   */
  inline void resized() noexcept override {}

protected:
  //==============================================================================
  /**
   * @brief Prepares the next frame for display.
   *
   * @details
   * Runs after each paint to update any state needed for the next frame.
   * Subclasses may override this to perform any necessary updates before the
   * next repaint. Do not draw or perform heavy calculations here.
   */
  virtual void prepareNextFrame() noexcept
  {
    // Default implementation does nothing and only exists to not force
    // subclasses to implement it if they don't need it
  }

private:
  //==============================================================================
  /**
   * @brief Called by the repaint timer to trigger a repaint.
   *
   * @details
   * This override is final and cannot be changed by subclasses.
   */
  inline void repaintTimerCallback() noexcept override final
  {
    this->repaint();
  }

  //==============================================================================
  // Members
  String displayName;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractDisplay)
};

} // namespace component
} // namespace gui
} // namespace dmt