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
 * Layout is a GUI component that manages the arrangement of panels components
 * in a grid layout. It allows for flexible panel placement and resizing based
 * on specified column and row separators.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/gui/panel/AbstractPanel.h"

//==============================================================================

namespace dmt {
namespace gui {
namespace window {

//==============================================================================

using GridSeparatorLayout = std::vector<float>;

//==============================================================================

class Layout : public juce::Component
{
  using AbstractPanel = dmt::gui::panel::AbstractPanel;
  using PanelPtr = std::shared_ptr<dmt::gui::panel::AbstractPanel>;
  using PanelList = std::vector<PanelPtr>;
  using PanelSpan = std::tuple<size_t, size_t, size_t, size_t>;
  using PanelSpanList = std::vector<PanelSpan>;

public:
  // Constructor takes column and row separators
  Layout(const GridSeparatorLayout _columnSeparators,
         const GridSeparatorLayout _rowSeparators)
    : columnSeparators(_columnSeparators)
    , rowSeparators(_rowSeparators)
  {
  }

  virtual ~Layout() = default;

  // JUCE paint
  void paint(juce::Graphics& g) override {}

  // JUCE resized
  void resized() override
  {
    auto bounds = getLocalBounds().toFloat();
    // Always treat 0.0 and 1.0 as implicit grid borders
    std::vector<float> cols = { 0.0f };
    cols.insert(cols.end(), columnSeparators.begin(), columnSeparators.end());
    cols.push_back(1.0f);

    std::vector<float> rows = { 0.0f };
    rows.insert(rows.end(), rowSeparators.begin(), rowSeparators.end());
    rows.push_back(1.0f);

    for (size_t i = 0; i < panels.size(); ++i) {
      auto [startCol, startRow, endCol, endRow] = panelSpans[i];
      float x0 = cols[startCol] * bounds.getWidth();
      float x1 = cols[endCol] * bounds.getWidth();
      float y0 = rows[startRow] * bounds.getHeight();
      float y1 = rows[endRow] * bounds.getHeight();
      juce::Rectangle<int> panelBounds(static_cast<int>(x0),
                                       static_cast<int>(y0),
                                       static_cast<int>(x1 - x0),
                                       static_cast<int>(y1 - y0));
      panels[i]->setBounds(panelBounds);
    }
  }

  /**
   * Adds a new panel of type PanelType to the layout grid.
   *
   * Template Parameters:
   * - PanelType: The specific panel class to instantiate, must derive from
   * AbstractPanel.
   * - Args: The types of constructor arguments to forward to PanelType's
   * constructor.
   *
   * This uses a variadic template (Args&&... _args) to forward any number
   * of constructor arguments into the PanelType constructor via std::forward.
   *
   * This makes the function flexible enough to handle any panel type with any
   * constructor signature, while preserving move/copy semantics.
   *
   * Example usage:
   *   addPanel<OscPanel>(0, 0, 1, 1, "waveform_saw", 42);
   *   -> constructs OscPanel("waveform_saw", 42)
   */
  template<typename PanelType, typename... Args>
  void addPanel(size_t _startCol,
                size_t _startRow,
                size_t _endCol,
                size_t _endRow,
                Args&&... _args)
  {
    static_assert(std::is_base_of<AbstractPanel, PanelType>::value,
                  "PanelType must derive from Panel");
    auto panel = std::make_shared<PanelType>(std::forward<Args>(_args)...);
    panels.push_back(panel);
    panelSpans.emplace_back(_startCol, _startRow, _endCol, _endRow);
    addAndMakeVisible(panel.get());
  }

protected:
  PanelList panels;
  // Stores (startCol, startRow, endCol, endRow) for each panel
  PanelSpanList panelSpans;
  GridSeparatorLayout columnSeparators;
  GridSeparatorLayout rowSeparators;
};

} // namespace window
} // namespace gui
} // namespace dmt