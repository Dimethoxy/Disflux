// #pragma once
// //==============================================================================
// #include <JuceHeader.h>
// #include <theme/StyleSheet.h>
// #include <utility/Settings.h>
// //==============================================================================
// namespace dmt {
// namespace theme {
// //==============================================================================
// class Loader
// {
//   using Settings = dmt::Settings;
//   using Panel = Settings::Panel;
//   using Carousel = Settings::Carousel;
//   using Slider = Settings::Slider;
//   using TriangleButton = Settings::TriangleButton;
//   using OscillatorDisplay = Settings::OscillatorDisplay;
//   using Oscilloscope = Settings::Oscilloscope;

// public:
//   //============================================================================
//   Loader() = delete;
//   //============================================================================
//   Loader(const Loader& obj) = delete;
//   //============================================================================
//   ~Loader() = delete;
//   //============================================================================
//   static void applyTheme(StyleSheet& sheet)
//   {
//     applyPanel(sheet);
//     applyCarousel(sheet);
//     applySlider(sheet);
//     applyTriangleButton(sheet);
//     applyOscillatorDisplay(sheet);
//     applyOscilloscope(sheet);
//   }

// protected:
//   //============================================================================
//   static void applyPanel(StyleSheet& sheet)
//   {
//     // General
//     sheet.applyFloat(Panel::padding, "Panel", "padding");
//     sheet.applyFloat(Panel::cornerSize, "Panel", "cornerSize");
//     // Border
//     sheet.applyBool(Panel::drawBorder, "Panel", "drawBorder");
//     sheet.applyColour(Panel::borderColour, "Panel", "borderColour");
//     sheet.applyFloat(Panel::borderStrength, "Panel", "borderStrength");
//     sheet.applyColour(Panel::backgroundColour, "Panel", "backgroundColour");
//     // Shadows
//     sheet.applyBool(Panel::drawOuterShadow, "Panel", "drawOuterShadow");
//     sheet.applyBool(Panel::drawInnerShadow, "Panel", "drawInnerShadow");
//     sheet.applyColour(Panel::outerShadowColour, "Panel",
//     "outerShadowColour"); sheet.applyColour(Panel::innerShadowColour,
//     "Panel", "innerShadowColour"); sheet.applyFloat(Panel::outerShadowRadius,
//     "Panel", "outerShadowRadius"); sheet.applyFloat(Panel::innerShadowRadius,
//     "Panel", "innerShadowRadius");
//     // Text
//     sheet.applyColour(Panel::fontColor, "Panel", "fontColor");
//     sheet.applyFloat(Panel::fontSize, "Panel", "fontSize");
//   }
//   //============================================================================
//   static void applyCarousel(StyleSheet& sheet)
//   {
//     sheet.applyFloat(Carousel::buttonWidth, "Carousel", "buttonWidth");
//     sheet.applyFloat(Carousel::buttonHeight, "Carousel", "buttonHeight");
//   }
//   //============================================================================
//   static void applySlider(StyleSheet& sheet)
//   {
//     // General
//     sheet.applyFloat(Slider::padding, "Slider", "padding");
//     sheet.applyFloat(Slider::baseWidth, "Slider", "baseWidth");
//     sheet.applyFloat(Slider::baseHeight, "Slider", "baseHeight");
//     sheet.applyFloat(Slider::sliderSize, "Slider", "sliderSize");
//     sheet.applyFloat(Slider::labelsSize, "Slider", "labelsSize");
//     // Font
//     sheet.applyColour(Slider::titleFontColour, "Slider", "titleFontColour");
//     sheet.applyColour(Slider::infoFontColour, "Slider", "infoFontColour");
//     sheet.applyFloat(Slider::titleFontSize, "Slider", "titleFontSize");
//     sheet.applyFloat(Slider::infoFontSize, "Slider", "infoFontSize");
//     // Shaft
//     sheet.applyColour(Slider::shaftColour, "Slider", "shaftColour");
//     sheet.applyFloat(Slider::shaftLineStrength, "Slider",
//     "shaftLineStrength"); sheet.applyFloat(Slider::shaftSize, "Slider",
//     "shaftSize");
//     // Rail
//     sheet.applyColour(Slider::lowerRailColour, "Slider", "lowerRailColour");
//     sheet.applyColour(Slider::upperRailColour, "Slider", "upperRailColour");
//     sheet.applyFloat(Slider::railWidth, "Slider", "railWidth");
//     sheet.applyFloat(Slider::railSize, "Slider", "railSize");
//     // Thumb
//     sheet.applyColour(Slider::thumbInnerColour, "Slider",
//     "thumbInnerColour"); sheet.applyColour(Slider::thumOuterColour, "Slider",
//     "thumOuterColour"); sheet.applyFloat(Slider::thumbSize, "Slider",
//     "thumbSize"); sheet.applyFloat(Slider::thumbStrength, "Slider",
//     "thumbStrength");
//     // Selections
//     sheet.applyColour(
//       Slider::selectionOuterColour, "Slider", "selectionOuterColour");
//     sheet.applyColour(
//       Slider::selectionInnerColour, "Slider", "selectionInnerColour");
//     sheet.applyColour(
//       Slider::selectionActiveColour, "Slider", "selectionActiveColour");
//     sheet.applyFloat(Slider::selectionWidth, "Slider", "selectionWidth");
//     sheet.applyFloat(Slider::selectionSize, "Slider", "selectionSize");
//     sheet.applyFloat(
//       Slider::selectionActivePadding, "Slider", "selectionActivePadding");
//   }
//   //============================================================================
//   static void applyTriangleButton(StyleSheet& sheet)
//   {
//     // General
//     sheet.applyColour(
//       TriangleButton::standbyColour, "TriangleButton", "standbyColour");
//     sheet.applyColour(
//       TriangleButton::hoverColour, "TriangleButton", "hoverColour");
//     sheet.applyFloat(TriangleButton::margin, "TriangleButton", "margin");
//     sheet.applyFloat(
//       TriangleButton::toggleReduction, "TriangleButton", "toggleReduction");
//     // Border
//     sheet.applyBool(TriangleButton::drawBorder, "TriangleButton",
//     "drawBorder"); sheet.applyColour(
//       TriangleButton::borderColour, "TriangleButton", "borderColour");
//     sheet.applyFloat(
//       TriangleButton::borderStrength, "TriangleButton", "borderStrength");
//     // Shadows
//     sheet.applyBool(
//       TriangleButton::drawOuterShadow, "TriangleButton", "drawOuterShadow");
//     sheet.applyBool(
//       TriangleButton::drawInnerShadow, "TriangleButton", "drawInnerShadow");
//     sheet.applyColour(
//       TriangleButton::outerShadowColour, "TriangleButton",
//       "outerShadowColour");
//     sheet.applyColour(
//       TriangleButton::innerShadowColour, "TriangleButton",
//       "innerShadowColour");
//     sheet.applyFloat(
//       TriangleButton::outerShadowRadius, "TriangleButton",
//       "outerShadowRadius");
//     sheet.applyFloat(
//       TriangleButton::innerShadowRadius, "TriangleButton",
//       "innerShadowRadius");
//   }
//   //============================================================================
//   static void applyOscillatorDisplay(StyleSheet& sheet)
//   {
//     // General
//     sheet.applyInt(
//       OscillatorDisplay::resolution, "OscillatorDisplay", "resolution");
//     // Shadow
//     sheet.applyBool(OscillatorDisplay::drawOuterShadow,
//                     "OscillatorDisplay",
//                     "drawOuterShadow");
//     sheet.applyBool(OscillatorDisplay::drawInnerShadow,
//                     "OscillatorDisplay",
//                     "drawInnerShadow");
//     sheet.applyColour(OscillatorDisplay::outerShadowColour,
//                       "OscillatorDisplay",
//                       "outerShadowColour");
//     sheet.applyColour(OscillatorDisplay::innerShadowColour,
//                       "OscillatorDisplay",
//                       "innerShadowColour");
//     sheet.applyFloat(OscillatorDisplay::outerShadowRadius,
//                      "OscillatorDisplay",
//                      "outerShadowRadius");
//     sheet.applyFloat(OscillatorDisplay::innerShadowRadius,
//                      "OscillatorDisplay",
//                      "innerShadowRadius");
//   }
//   //============================================================================
//   static void applyOscilloscope(StyleSheet& sheet)
//   {
//     // General
//     sheet.applyColour(
//       Oscilloscope::backgroundColour, "Oscilloscope", "backgroundColour");
//     sheet.applyFloat(Oscilloscope::padding, "Oscilloscope", "padding");
//     sheet.applyFloat(Oscilloscope::cornerSize, "Oscilloscope", "cornerSize");
//     // Border
//     sheet.applyBool(Oscilloscope::drawBorder, "Oscilloscope", "drawBorder");
//     sheet.applyColour(
//       Oscilloscope::borderColour, "Oscilloscope", "borderColour");
//     sheet.applyFloat(
//       Oscilloscope::borderStrength, "Oscilloscope", "borderStrength");
//     // Shadow
//     sheet.applyBool(
//       Oscilloscope::drawOuterShadow, "Oscilloscope", "drawOuterShadow");
//     sheet.applyBool(
//       Oscilloscope::drawInnerShadow, "Oscilloscope", "drawInnerShadow");
//     sheet.applyColour(
//       Oscilloscope::outerShadowColour, "Oscilloscope", "outerShadowColour");
//     sheet.applyColour(
//       Oscilloscope::innerShadowColour, "Oscilloscope", "innerShadowColour");
//     sheet.applyFloat(
//       Oscilloscope::outerShadowRadius, "Oscilloscope", "outerShadowRadius");
//     sheet.applyFloat(
//       Oscilloscope::innerShadowRadius, "Oscilloscope", "innerShadowRadius");
//   }
// };
// //==============================================================================
// } // namespace theme
// } // namespace dmt