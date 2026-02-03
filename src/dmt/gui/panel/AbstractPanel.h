//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ███████ ██    ██   ███     ████
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
 * AbstractPanel provides a base class for all panel components in the GUI,
 * encapsulating layout, grid, border, shadow, and navigation logic.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dmt/utility/Scaleable.h"
#include "gui/widget/Label.h"
#include "gui/widget/Shadow.h"
#include "gui/widget/TriangleButton.h"
#include "utility/Fonts.h"
#include "utility/Settings.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace panel {

//==============================================================================
/**
 * @brief Abstract base class for GUI panels with grid layout, navigation, and
 * shadow/border rendering.
 *
 * @details
 * This class provides a flexible foundation for all panels in the GUI,
 * supporting grid-based layouts, navigation buttons, and customizable
 * appearance (shadows, borders, etc). Designed for real-time performance and
 * extensibility. Subclasses should override extendResize() and getName() for
 * custom behavior.
 */
class AbstractPanel
  : public juce::Component
  , public juce::Button::Listener
  , public dmt::Scaleable<AbstractPanel>
{
public:
  //==============================================================================
  using Grid = std::vector<std::vector<juce::Point<float>>>;
  using TriangleButton = dmt::gui::widget::TriangleButton;
  using Shadow = dmt::gui::widget::Shadow;
  using Label = dmt::gui::widget::Label;
  using Fonts = dmt::utility::Fonts;

  // Settings
  using LibrarySettings = dmt::Settings;
  using Settings = LibrarySettings::Panel;
  using Carousel = LibrarySettings::Carousel;

  // Layout
  const float& margin =
    LibrarySettings::Window::margin; // TODO: Make sure this is used properly
  // General
  const juce::Colour& backgroundColour = Settings::backgroundColour;
  const float& cornerSize = Settings::cornerSize;
  const float& rawPadding = Settings::padding;
  // Border
  const bool& drawBorder = Settings::drawBorder;
  const juce::Colour& borderColour = Settings::borderColour;
  const float& borderStrength = Settings::borderStrength;
  // Shadows
  const bool& drawOuterShadow = Settings::drawOuterShadow;
  const bool& drawInnerShadow = Settings::drawInnerShadow;
  const juce::Colour& outerShadowColour = Settings::outerShadowColour;
  const juce::Colour& innerShadowColour = Settings::innerShadowColour;
  const float& outerShadowRadius = Settings::outerShadowRadius;
  const float& innerShadowRadius = Settings::innerShadowRadius;
  // Fonts
  const juce::Colour& fontColor = Settings::fontColor;
  const float& fontSize = Settings::fontSize;
  // Debug
  const bool& debugBounds = LibrarySettings::debugBounds;
  const bool& debugGrid = LibrarySettings::debugGrid;

  //==============================================================================
  /**
   * @brief Structure representing the grid layout (columns and rows).
   */
  struct Layout
  {
    int cols;
    int rows;
  };

  //==============================================================================
  /**
   * @brief Constructs an AbstractPanel.
   *
   * @param _name The panel's name (used for the title label).
   * @param _displayName If true, displays the title label.
   *
   * @details
   * Initializes layout, grid, title label, navigation buttons, and shadows.
   * The constructor is constexpr for maximum compile-time optimization.
   */
  explicit AbstractPanel(const juce::String _name,
                         const bool _displayName = true) noexcept
    : layout({ 1, 1 })
    , rawGridOffsetY(40 * static_cast<int>(_displayName))
    , name(_name)
    , titleLabel(_name, fonts.bold, fontSize, juce::Colours::white)
    , nextCallback([]() {})
    , prevCallback([]() {})
    , nextButton(dmt::gui::widget::TriangleButton::Right)
    , prevButton(dmt::gui::widget::TriangleButton::Left)
    , outerShadow(drawOuterShadow, outerShadowColour, outerShadowRadius, false)
    , innerShadow(drawInnerShadow, outerShadowColour, outerShadowRadius, true)
  {
    TRACER("AbstractPanel::AbstractPanel");
    if (_displayName) {
      addAndMakeVisible(titleLabel);
    }
    setLayout(layout);
    addAndMakeVisible(outerShadow);
    addAndMakeVisible(innerShadow);
  }

  //==============================================================================
  /**
   * @brief Paints the panel, including background, border, and debug overlays.
   *
   * @param _g The graphics context.
   *
   * @details
   * Draws the panel's background, border, and optional debug grid/bounds.
   * Uses settings for appearance. All drawing is performed in local
   * coordinates.
   */
  inline void paint(juce::Graphics& _g) noexcept override
  {
    TRACER("AbstractPanel::paint");
    // Precalculation
    const auto bounds = this->getLocalBounds().toFloat();
    const auto outerBounds = bounds.reduced(margin * size * 0.5f);
    const auto innerBounds = outerBounds.reduced(borderStrength * size);
    const float outerCornerSize = cornerSize * size;
    const float innerCornerSize = std::clamp(
      outerCornerSize - (borderStrength * size * 0.5f), 0.0f, outerCornerSize);

    // draw debug bounds
    _g.setColour(juce::Colours::aqua);
    if (debugBounds)
      _g.drawRect(bounds, 1.0f);

    // Draw background if border is disabled
    if (!drawBorder) {
      _g.setColour(backgroundColour);
      _g.fillRoundedRectangle(outerBounds, outerCornerSize);
    }

    // Draw background and border if border is enabled
    if (drawBorder) {
      _g.setColour(borderColour);
      _g.fillRoundedRectangle(outerBounds, outerCornerSize);
      _g.setColour(backgroundColour);
      _g.fillRoundedRectangle(innerBounds, innerCornerSize);
    }

    // draw debug line grid
    if (debugGrid) {
      _g.setColour(juce::Colours::red);
      for (size_t col = 0; col < grid.size(); col++) {
        const auto firstPoint =
          getGridPoint(bounds.toNearestInt(), static_cast<int>(col), 0);
        const auto endPoint = juce::Point<int>(
          firstPoint.getX(), static_cast<int>(bounds.getHeight()) - 1);
        const auto line =
          juce::Line<float>(firstPoint.toFloat(), endPoint.toFloat());
        _g.drawLine(line, 1.0f);
      }
      for (size_t row = 0; row < grid[0].size(); row++) {
        const auto firstPoint =
          getGridPoint(bounds.toNearestInt(), 0, static_cast<int>(row));
        const auto endPoint = juce::Point<int>(
          static_cast<int>(bounds.getWidth()) - 1, firstPoint.getY());
        const auto line =
          juce::Line<float>(firstPoint.toFloat(), endPoint.toFloat());
        _g.drawLine(line, 1.0f);
      }
    }
  }

  //==============================================================================
  /**
   * @brief Handles resizing of the panel and its subcomponents.
   *
   * @details
   * Updates bounds for shadows, navigation buttons, and the title label.
   * Calls extendResize() for subclass-specific resizing.
   */
  inline void resized() noexcept override
  {
    TRACER("AbstractPanel::resized");
    const auto bounds = getLocalBounds();
    const auto outerBounds = bounds.reduced(margin * size * 0.5f);
    const auto innerBounds = outerBounds.reduced(borderStrength * size);
    const float outerCornerSize = cornerSize * size;
    const float innerCornerSize = std::clamp(
      outerCornerSize - (borderStrength * size * 0.5f), 0.0f, outerCornerSize);

    juce::Path outerShadowPath;
    outerShadowPath.addRoundedRectangle(outerBounds, outerCornerSize);
    outerShadow.setPath(outerShadowPath);
    outerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    outerShadow.toBack();

    juce::Path innerShadowPath;
    innerShadowPath.addRoundedRectangle(innerBounds, innerCornerSize);
    innerShadow.setPath(innerShadowPath);
    innerShadow.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    innerShadow.toBack();

    const int buttonWidth = static_cast<int>(Carousel::buttonWidth * size);
    const int buttonHeight = static_cast<int>(Carousel::buttonHeight * size);
    const int marginSize = static_cast<int>(size * margin);
    const int padding = static_cast<int>(rawPadding * size);

    auto leftBounds = bounds;
    auto rightBounds = bounds;

    leftBounds.removeFromRight(bounds.getWidth() - buttonWidth);
    leftBounds.setHeight(buttonHeight);
    leftBounds.setCentre(leftBounds.getCentreX(), bounds.getCentreY());
    prevButton.setBounds(leftBounds.reduced(marginSize));

    rightBounds.removeFromLeft(bounds.getWidth() - buttonWidth);
    rightBounds.setHeight(buttonHeight);
    rightBounds.setCentre(rightBounds.getCentreX(), bounds.getCentreY());
    nextButton.setBounds(rightBounds.reduced(marginSize));

    titleLabel.setBounds(bounds.reduced(padding + marginSize));

    extendResize();
  }

  //==============================================================================
  /**
   * @brief Extension point for subclasses to handle additional resizing logic.
   *
   * @details
   * Override in subclasses to resize custom subcomponents.
   */
  virtual inline void extendResize() noexcept {}

  //==============================================================================
  /**
   * @brief Returns the panel's name.
   *
   * @return The name of the panel.
   *
   * @details
   * Override in subclasses to provide a custom name.
   */
  virtual inline const juce::String getName() noexcept
  {
    TRACER("AbstractPanel::getName");
    return "Panel";
  }

  //==============================================================================
  /**
   * @brief Sets the callbacks for navigation buttons.
   *
   * @param _next Callback for the "next" button.
   * @param _prev Callback for the "previous" button.
   *
   * @details
   * Registers the callbacks and makes the navigation buttons visible.
   */
  inline void setCallbacks(std::function<void()> _next,
                           std::function<void()> _prev)
  {
    TRACER("AbstractPanel::setCallbacks");
    nextCallback = _next;
    prevCallback = _prev;
    addAndMakeVisible(nextButton);
    addAndMakeVisible(prevButton);
    nextButton.addListener(this);
    prevButton.addListener(this);
  }

  //==============================================================================
  /**
   * @brief Invokes the "next" callback.
   */
  inline void next()
  {
    TRACER("AbstractPanel::next");
    nextCallback();
  }

  //==============================================================================
  /**
   * @brief Invokes the "previous" callback.
   */
  inline void prev()
  {
    TRACER("AbstractPanel::prev");
    prevCallback();
  }

  //==============================================================================
  /**
   * @brief Handles button click events for navigation.
   *
   * @param _button The button that was clicked.
   *
   * @details
   * Invokes the appropriate callback based on which navigation button was
   * pressed.
   */
  inline void buttonClicked(juce::Button* _button) override
  {
    TRACER("AbstractPanel::buttonClicked");
    if (_button == &nextButton) {
      nextCallback();
    } else if (_button == &prevButton) {
      prevCallback();
    }
  }

protected:
  //==============================================================================
  /**
   * @brief Returns the current grid layout.
   *
   * @return The current Layout struct.
   */
  [[nodiscard]] inline const Layout getLayout() noexcept
  {
    TRACER("AbstractPanel::getLayout");
    return layout;
  }

  //==============================================================================
  /**
   * @brief Sets the grid layout and recalculates grid points.
   *
   * @param _layoutToUse The desired layout (columns and rows).
   *
   * @details
   * Recomputes the grid for the new layout, ensuring all points are updated.
   */
  inline void setLayout(const Layout _layoutToUse) noexcept
  {
    TRACER("AbstractPanel::setLayout");
    const int cols = _layoutToUse.cols;
    const int rows = _layoutToUse.rows;
    const float colSpacing = 1.0f / static_cast<float>(cols + 1);
    const float rowSpacing = 1.0f / static_cast<float>(rows + 1);
    Grid newGrid(
      static_cast<size_t>(cols + 2),
      std::vector<juce::Point<float>>(static_cast<size_t>(rows + 2)));

    for (size_t col = 0; col <= static_cast<size_t>(_layoutToUse.cols + 1);
         col++) {
      for (size_t row = 0; row <= static_cast<size_t>(_layoutToUse.rows + 1);
           row++) {
        const float x = static_cast<float>(col) * colSpacing;
        const float y = static_cast<float>(row) * rowSpacing;
        const auto point = juce::Point<float>(x, y);
        newGrid[col][row] = point;
      }
    }
    this->grid = newGrid;
    this->layout = _layoutToUse;
  }

  //==============================================================================
  /**
   * @brief Returns the pixel position of a grid point within the given bounds.
   *
   * @param _bounds The bounding rectangle.
   * @param _col The column index.
   * @param _row The row index.
   * @return The pixel position as a juce::Point<int>.
   *
   * @details
   * Computes the position of a grid point, accounting for grid offset and
   * scaling. Asserts if indices are out of bounds.
   */
  [[nodiscard]] inline const juce::Point<int> getGridPoint(
    const juce::Rectangle<int> _bounds,
    const int _col,
    const int _row) noexcept
  {
    TRACER("AbstractPanel::getGridPoint");
    // assert if col and row are out of bounds
    jassert(_col >= 0 && static_cast<size_t>(_col) < grid.size());
    jassert(_row >= 0 &&
            static_cast<size_t>(_row) < grid[static_cast<size_t>(_col)].size());

    auto rawPoint = grid[static_cast<size_t>(_col)][static_cast<size_t>(_row)];
    const auto x = rawPoint.getX() * static_cast<float>(_bounds.getWidth());

    const float gridOffsetY = static_cast<float>(rawGridOffsetY) * size;
    const float offsetBoundsHeight =
      static_cast<float>(_bounds.getHeight()) - gridOffsetY;
    const auto y = rawPoint.getY() * offsetBoundsHeight + gridOffsetY;

    juce::Point<float> point(x, y);
    return point.toInt();
  }

  //==============================================================================
  /**
   * @brief Sets the raw Y offset for the grid.
   *
   * @param _offset The offset in pixels.
   *
   * @details
   * Used to vertically shift the grid, e.g., to make space for a title.
   */
  inline void setRawGridOffset(const int _offset) noexcept
  {
    TRACER("AbstractPanel::setRawGridOffset");
    rawGridOffsetY = _offset;
  }

  //==============================================================================
  /**
   * @brief Returns the current raw grid Y offset.
   *
   * @return The raw grid Y offset in pixels.
   */
  [[nodiscard]] inline const int getRawGridOffset() const noexcept
  {
    TRACER("AbstractPanel::getRawGridOffset");
    return rawGridOffsetY;
  }

private:
  //==============================================================================
  // Members initialized in the initializer list
  Layout layout;
  int rawGridOffsetY;
  const juce::String name;
  Label titleLabel;
  std::function<void()> nextCallback;
  std::function<void()> prevCallback;
  TriangleButton nextButton;
  TriangleButton prevButton;
  Shadow outerShadow;
  Shadow innerShadow;

  //==============================================================================
  // Other members
  Grid grid;
  Fonts fonts;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractPanel)
};

} // namespace panel
} // namespace gui
} // namespace dmt