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
 * PitchPanel is a GUI component that provides controls for pitch settings.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/panel/AbstractPanel.h"
#include "gui/panel/AnalogPitchPanel.h"
#include "gui/panel/Carousel.h"
#include "gui/panel/ModernPitchPanel.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace panel {

//==============================================================================
class PitchPanel : public dmt::gui::panel::Carousel
{
public:
  PitchPanel(juce::AudioProcessorValueTreeState& apvts)
    : Carousel()
  {
    panels.push_back(std::make_unique<AnalogPitchPanel>(apvts));
    panels.push_back(std::make_unique<ModernPitchPanel>());
    init();
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchPanel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt
