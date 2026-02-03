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
 * Get the options for the properties file with predefined settings.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace dsp {
namespace synth {

//==============================================================================
/**
 * @brief Represents different types of analog waveforms.
 */
struct AnalogWaveform
{
  static constexpr float twoPi = juce::MathConstants<float>::twoPi;
  static constexpr float pi = juce::MathConstants<float>::pi;

  //==============================================================================
  /**
   * @brief Enumeration of waveform types.
   */
  enum class Type
  {
    Sine,
    Saw,
    Triangle,
    Square
  };

  static const inline juce::StringArray waveformNames = { "Sine",
                                                          "Saw",
                                                          "Triangle",
                                                          "Square" };

  //==============================================================================

  Type type = Type::Sine;

  //==============================================================================
  /**
   * @brief Generate a triangle waveform sample.
   * @param _x The phase of the waveform.
   * @return The waveform sample.
   */
  inline float triangle(float _x) const noexcept
  {
    while (_x > twoPi)
      _x -= twoPi;
    float result = 2.0f * (_x / twoPi - 0.5f);
    if (result > 0.5f)
      result = 1.0f - result;
    if (result < -0.5f)
      result = -1.0f - result;
    return 2 * result;
  }

  //==============================================================================
  /**
   * @brief Generate a saw waveform sample.
   * @param _x The phase of the waveform.
   * @return The waveform sample.
   */
  inline float saw(float _x) const noexcept
  {
    while (_x > twoPi)
      _x -= twoPi;
    return 2.0f * (_x / twoPi - 0.5f);
  }

  //==============================================================================
  /**
   * @brief Generate a sine waveform sample.
   * @param _x The phase of the waveform.
   * @return The waveform sample.
   */
  inline float sine(float _x) const noexcept { return std::sin(_x); }

  //==============================================================================
  /**
   * @brief Generate a square waveform sample.
   * @param _x The phase of the waveform.
   * @return The waveform sample.
   */
  inline float square(float _x) const noexcept
  {
    return (sine(_x) > 0.0f) ? 1.0f : -1.0f;
  }

  //==============================================================================
  /**
   * @brief Get the waveform sample based on the current type.
   * @param _x The phase of the waveform.
   * @return The waveform sample.
   */
  [[nodiscard]] inline float getSample(float _x) const noexcept
  {
    switch (type) {
      case Type::Sine:
        return sine(_x);
      case Type::Saw:
        return saw(_x);
      case Type::Triangle:
        return triangle(_x);
      case Type::Square:
        return square(_x);
      default:
        // impossible to reach this point, exit with assertion
        jassert(false);
        return 0.0f;
    }
  }

  //==============================================================================
};

//==============================================================================

} // namespace synth
} // namespace dsp
} // namespace dmt

//==============================================================================
