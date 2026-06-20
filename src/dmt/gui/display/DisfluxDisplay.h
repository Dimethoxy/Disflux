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
 * You are not allowed to use this code in any closed-source project.
 *
 * Description:
 * Specialized display component inheriting from AbstractDisplay.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dsp/data/FifoAudioBuffer.h"
#include "gui/display/MultiDisplay.h"
#include "gui/display/OscilloscopeDisplay.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {
//==============================================================================

/**
 * @class DisfluxDisplay
 * @brief Specialized display component inheriting from
 * AbstractDisplay.
 */
class alignas(64) DisfluxDisplay : public dmt::gui::display::MultiDisplay
{
  using MultiDisplay = dmt::gui::display::MultiDisplay;
  using DisfluxProcessor = dmt::dsp::effect::DisfluxProcessor;

public:
  using FifoAudioBuffer = dmt::dsp::data::FifoAudioBuffer<float>;
  DisfluxDisplay(FifoAudioBuffer& _fifoBuffer,
                 AudioProcessorValueTreeState& _apvts)
    : MultiDisplay(
        _apvts,
        [&] {
          std::vector<std::unique_ptr<AbstractDisplay>> displays;
          displays.push_back(std::make_unique<OscilloscopeDisplay<float>>(
            _fifoBuffer, _apvts, true));
          return displays;
        }(),
        // Display mapper
        { { "", "" } })
  {
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisfluxDisplay)
};
} // namespace display
} // namespace gui
} // namespace dmt