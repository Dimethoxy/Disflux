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
 * Carousel is a GUI component that manages multiple AbstractPanel instances,
 * allowing navigation between them using next and previous buttons.
 *
 * Authors: Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "gui/panel/AbstractPanel.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace panel {

//==============================================================================
class Carousel : public juce::Component
{
public:
  Carousel()
    : index(0)
  {
  }
  void next()
  {
    panels[index]->setVisible(false);
    index++;
    if (index >= panels.size())
      index -= (int)panels.size();
    panels[index]->setVisible(true);
    repaint();
  }
  void previous()
  {
    panels[index]->setVisible(false);
    index--;
    if (index < 0)
      index += (int)panels.size();
    panels[index]->setVisible(true);
    repaint();
  }
  void init()
  {
    for (auto& panel : panels) {
      panel->setCallbacks([this]() { next(); }, [this]() { previous(); });
      addChildComponent(*panel);
    }
    panels[index]->setVisible(true);
  }

  void resized() override
  {
    for (auto& panel : panels) {
      panel->setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    }
  }

protected:
  std::vector<std::unique_ptr<AbstractPanel>> panels;

private:
  int index;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Carousel)
};
//==============================================================================
} // namespace panels
} // namespace gui
} // namespace dmt