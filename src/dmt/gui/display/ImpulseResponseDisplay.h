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
 * You must adhere GPLv3 license for any project and parts of it.
 * You are not allowed to use this code in any closed-source project.
 *
 * Description:
 * ImpulseResponseDisplay is a standard juce::Component shell (via
 * AbstractDisplay). It owns the layout, labels, data, and DisplayChrome.
 * All audio-data rendering is delegated to a DisplayRenderer strategy
 * passed at construction time. This class contains zero backend-specific
 * rendering code.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/display/AbstractDisplay.h"
#include "gui/display/ImpulseResponseBuilder.h"
#include "gui/widget/Label.h"
#include "utility/Fonts.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace display {

//==============================================================================

template<typename ProcessorType>
class ImpulseResponseDisplay : public dmt::gui::display::AbstractDisplay
{
  using Label = dmt::gui::widget::Label;
  using Fonts = dmt::utility::Fonts;
  using Settings = dmt::Settings;

public:
  explicit ImpulseResponseDisplay(
    ProcessorType& _processor,
    juce::AudioProcessorValueTreeState& _apvts,
    int _impulseSize,
    juce::Array<juce::Identifier> _parametersToWatch)
    : AbstractDisplay("Impulse Response")
    , impulseResponseBuilder(_processor,
                             _apvts,
                             _impulseSize,
                             _parametersToWatch)
  {
  }

  //==========================================================================
  // Component lifecycle

  void resized() noexcept override {}

  //==========================================================================
  void paint(juce::Graphics& _g) noexcept override {}

private:
  ImpulseResponseBuilder<ProcessorType> impulseResponseBuilder;

  juce::OpenGLContext openGLContext;

  //==========================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImpulseResponseDisplay)
};

//==============================================================================

} // namespace display
} // namespace gui
} // namespace dmt

//==============================================================================