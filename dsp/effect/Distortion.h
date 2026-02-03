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
 * modify this code under the terms of this license. You must adhere GPLv3
 * license for any project using this code or parts of it. Your are not allowed
 * to use this code in any closed-source project.
 *
 * Description:
 * Distortion effect processor.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

//==============================================================================

namespace dmt {
namespace dsp {
namespace effect {

//==============================================================================

/**
 * @brief Distortion effect processor
 *
 * This class provides various types of distortion effects.
 * It is optimized for real-time performance.
 */
struct alignas(64) Distortion
{
  enum class Type
  {
    Hardclip,
    Softclip,
    Saturate,
    Atan,
    Crunch,
    Extreme,
    Scream,
    Sine,
    Cosine,
    Harmonize,
    Weird,
    Bitcrush,
  };

  //==============================================================================
  /**
   * @brief Get the string representation of the distortion type.
   *
   * @param _type The distortion type.
   * @return The string representation of the type.
   */
  [[nodiscard]] static inline const juce::String getString(
    const Type _type) noexcept
  {
    switch (_type) {
      case Distortion::Type::Hardclip:
        return "Hardclip";
      case Distortion::Type::Softclip:
        return "Softclip";
      case Distortion::Type::Saturate:
        return "Saturate";
      case Distortion::Type::Atan:
        return "Atan";
      case Distortion::Type::Crunch:
        return "Crunch";
      case Distortion::Type::Extreme:
        return "Extreme";
      case Distortion::Type::Scream:
        return "Scream";
      case Distortion::Type::Sine:
        return "Sine";
      case Distortion::Type::Cosine:
        return "Cosine";
      case Distortion::Type::Harmonize:
        return "Harmonize";
      case Distortion::Type::Weird:
        return "Weird";
      case Distortion::Type::Bitcrush:
        return "Bitcrush";
      default:
        jassertfalse;
        return "Hardclip";
    }
  }

  //==============================================================================
  /**
   * @brief Apply distortion to a sample.
   *
   * @param _data The sample data.
   * @param _type The distortion type.
   * @param _drive The drive amount.
   */
  static inline void distortSample(float& _data,
                                   const Type _type,
                                   const float _drive) noexcept
  {
    switch (_type) {
      case Type::Hardclip:
        _data = std::clamp(_drive * _data, -1.0f, 1.0f);
        break;
      case Type::Softclip: {
        constexpr float threshold1 = 1.0f / 3.0f;
        constexpr float threshold2 = 2.0f / 3.0f;

        _data *= _drive;
        if (_data > threshold2)
          _data = 1.0f;
        else if (_data > threshold1)
          _data = 1.0f - std::pow(2.0f - 3.0f * _data, 2.0f) / 3.0f;
        else if (_data < -threshold2)
          _data = -1.0f;
        else if (_data < -threshold1)
          _data = -1.0f + std::pow(2.0f + 3.0f * _data, 2.0f) / 3.0f;
        else
          _data = 2.0f * _data;
        break;
      }

      case Type::Saturate:
        if (_data > 0.0f) {
          _data = std::clamp(
            std::pow(_data, 1.0f / ((_drive / 4.0f) + 0.75f)), -1.0f, 1.0f);
        } else {
          _data = -std::clamp(
            std::pow(-_data, 1.0f / ((_drive / 4.0f) + 0.75f)), -1.0f, 1.0f);
        }
        break;
      case Type::Atan:
        if (juce::approximatelyEqual(_data, 0.0f)) {
          if (_data > 0.0f) {
            _data = std::pow(_data, 1.0f / _drive);
            _data = 1.27f * std::atan(_data);
          } else {
            _data = std::pow(-_data, 1.0f / _drive);
            _data = 1.27f * std::atan(_data);
            _data = -_data;
          }
        }
        break;
      case Type::Crunch:
        if (_data > 0.0f) {
          _data = std::pow(_data, 1.0f / _drive);
          _data = 1.27f * std::atan(_data);
        } else {
          _data = std::clamp(std::sin(_drive * _data), -1.0f, 1.0f);
          _data = std::clamp(_drive * _data, -1.0f, 1.0f);
        }
        break;
      case Type::Extreme: {
        const float invertedDrive = 10.0f - (_drive - 1.0f);
        if (std::abs(_data) >= ((invertedDrive - 1.0f) / 9.0f)) {
          _data = std::signbit(_data) ? -1.0f : 1.0f;
        }
        break;
      }
      case Type::Scream: {
        auto temp = _data;
        const float normalizedDrive = (_drive - 1.0f) / 10.0f;

        if (_data > 0.0f) {
          _data = std::clamp(
            std::pow(_data, 1.0f / ((_drive / 4.0f) + 0.75f)), -1.0f, 1.0f);
        } else {
          _data = -std::clamp(
            std::pow(-_data, 1.0f / ((_drive / 4.0f) + 0.75f)), -1.0f, 1.0f);
        }

        if (_data <= -0.5f) {
          constexpr float offset = 3.0f;
          temp = 4.0f * _data + offset;
        } else if (_data > -0.5f && _data < 0.5f) {
          temp = -2.0f * _data;
        } else {
          constexpr float offset = -3.0f;
          temp = 4.0f * _data + offset;
        }

        _data = (temp * normalizedDrive) + (_data * (1.0f - normalizedDrive));
        break;
      }
      case Type::Sine:
        _data = std::clamp(std::sin(_drive * _data), -1.0f, 1.0f);
        break;
      case Type::Cosine:
        _data = std::clamp(std::cos(_drive * _data), -1.0f, 1.0f);
        break;
      case Type::Harmonize: {
        _data *= _drive * 5.0f;
        const float h1 = std::sin(2.0f * _data);
        const float h2 = std::sin(3.0f * _data);
        const float h3 = std::sin(4.0f * _data);
        _data = (h1 + h2 + h3 + _data) / (_drive * 5.0f);
        break;
      }
      case Type::Weird: {
        _data *= _drive * 2.0f;
        const float h1 = std::sin(2.0f * _data);
        const float h2 = std::sin(3.0f * _data);
        const float h3 = std::sin(4.0f * _data);
        _data = std::sin(h1 + h2 + h3 + _data);
        break;
      }
      case Type::Bitcrush: {
        const float bitDepth = 10.0f - (_drive - 1.0f);
        const float exponent = bitDepth - 1.0f;
        const float possibleValues = std::pow(2.0f, exponent);
        float quantized = (_data + 1.0f) * possibleValues;
        quantized = std::round(quantized);
        _data = (quantized / possibleValues) - 1.0f;
        break;
      }
    }
  }

  //==============================================================================
  /**
   * @brief Generate a new random seed for girth effect.
   *
   * @return The new girth seed.
   */
  [[nodiscard]] static inline float getNewGirthSeed() noexcept
  {
    static thread_local std::mt19937 generator{ std::random_device{}() };
    static thread_local std::uniform_real_distribution<float> distribution(
      0.0f, 100.0f);
    return distribution(generator);
  }

  //==============================================================================
  /**
   * @brief Generate a vector of girth seeds.
   *
   * @param _numSamples The number of samples.
   * @return A vector of girth seeds.
   */
  [[nodiscard]] static inline std::vector<float> getGirthSeeds(
    const int _numSamples) noexcept
  {
    std::vector<float> girthSeeds;
    girthSeeds.reserve(static_cast<size_t>(_numSamples));
    for (int i = 0; i < _numSamples; ++i) {
      girthSeeds.push_back(getNewGirthSeed());
    }
    return girthSeeds;
  }

  //==============================================================================
  /**
   * @brief Apply girth effect to a sample.
   *
   * @param _value The sample value.
   * @param _girth The girth amount.
   */
  static inline void girthSample(float& _value, const float _girth) noexcept
  {
    _value *= ((getNewGirthSeed() / 100.0f * _girth) + 1.0f);
    _value = std::clamp(_value, -1.0f, 1.0f);
  }

  //==============================================================================
  /**
   * @brief Apply girth effect to a sample with a specific seed.
   *
   * @param _value The sample value.
   * @param _girth The girth amount.
   * @param _seed The girth seed.
   */
  static inline void girthSample(float& _value,
                                 const float _girth,
                                 const float _seed) noexcept
  {
    _value *= ((_seed / 100.0f * _girth) + 1.0f);
    _value = std::clamp(_value, -1.0f, 1.0f);
  }

  //==============================================================================
  /**
   * @brief Apply symmetry effect to a sample.
   *
   * @param _value The sample value.
   * @param _symmetry The symmetry amount.
   */
  static inline void symmetrySample(float& _value,
                                    const float _symmetry) noexcept
  {
    if (_value > 0.0f) {
      _value += _value * _symmetry;
    } else {
      _value -= _value * _symmetry;
    }
    _value = std::clamp(_value, -1.0f, 1.0f);
  }

  //==============================================================================
  /**
   * @brief Process an audio buffer with distortion, girth, and symmetry
   * effects.
   *
   * @param _buffer The audio buffer.
   * @param _type The distortion type.
   * @param _symmetry The symmetry amount.
   * @param _girth The girth amount.
   * @param _drive The drive amount.
   */
  static inline void processBuffer(juce::AudioBuffer<float>& _buffer,
                                   const Type _type,
                                   const float _symmetry,
                                   const float _girth,
                                   const float _drive) noexcept
  {
    std::vector<float> girthSeeds;
    if (_girth < 0.0f) {
      girthSeeds = getGirthSeeds(_buffer.getNumSamples());
    }

    for (int channel = 0; channel < _buffer.getNumChannels(); ++channel) {
      auto* channelData = _buffer.getWritePointer(channel);
      for (int sample = 0; sample < _buffer.getNumSamples(); ++sample) {
        if (_girth < 0.0f) {
          girthSample(
            channelData[sample], std::abs(_girth), girthSeeds[sample]);
        } else {
          girthSample(channelData[sample], _girth);
        }

        distortSample(channelData[sample], _type, _drive);
        symmetrySample(channelData[sample], _symmetry);
      }
    }
  }
};

//==============================================================================
} // namespace effect
} // namespace dsp
} // namespace dmt