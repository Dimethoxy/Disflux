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

#include "utility/Math.h"

//==============================================================================

namespace dmt {
namespace dsp {
namespace envelope {

//==============================================================================

/**
 * @brief AHD Envelope Generator
 *
 * This class generates an Attack-Hold-Decay (AHD) envelope.
 * It is optimized for real-time performance.
 */
class AhdEnvelope
{
public:
  struct Parameters
  {
    float attack = 0.015f;
    float hold = 0.08f;
    float decay = 0.5f;

    float attackSkew = 0;
    float decaySkew = 10;
  };

  enum class State
  {
    Attack,
    Hold,
    Decay,
    Idle
  };

  constexpr AhdEnvelope() noexcept = default;

  /**
   * @brief Set the envelope parameters.
   * @param _newParams The new parameters to set.
   */
  inline void setParameters(const Parameters& _newParams) noexcept
  {
    params = _newParams;
  }

  /**
   * @brief Set the sample rate.
   * @param _newSampleRate The new sample rate to set.
   */
  inline void setSampleRate(const float _newSampleRate) noexcept
  {
    sampleRate = _newSampleRate;
  }

  /**
   * @brief Trigger the envelope to start.
   */
  inline void noteOn() noexcept { sampleIndex = 0; }

  /**
   * @brief Get the current state of the envelope.
   * @return The current state.
   */
  [[nodiscard]] inline State getState() const noexcept
  {
    if (sampleIndex < getHoldStart()) [[likely]]
      return State::Attack;
    if (sampleIndex < getDecayStart()) [[likely]]
      return State::Hold;
    if (sampleIndex < getDecayEnd()) [[likely]]
      return State::Decay;
    return State::Idle;
  }

  /**
   * @brief Get the next sample value of the envelope.
   * @return The next sample value.
   */
  [[nodiscard]] inline float getNextSample() noexcept
  {
    const auto state = getState();
    const float result = getValue(state);
    ++sampleIndex;
    return result;
  }

private:
  /**
   * @brief Get the value of the envelope based on its state.
   * @param _state The current state of the envelope.
   * @return The value of the envelope.
   */
  [[nodiscard]] inline float getValue(const State _state) const noexcept
  {
    constexpr float one = 1.0f;
    constexpr float zero = 0.0f;

    switch (_state) {
      case State::Attack: {
        const float normalizedPosition =
          static_cast<float>(sampleIndex) / sampleRate;
        const float skew = getSkew(State::Attack);
        return std::pow(normalizedPosition / params.attack, skew);
      }
      case State::Hold:
        return one;
      case State::Decay: {
        const float decayStart = static_cast<float>(getDecayStart());
        const float normalizedPosition =
          (static_cast<float>(sampleIndex) - decayStart) / sampleRate;
        const float skew = getSkew(State::Decay);
        return one - std::pow(normalizedPosition / params.decay, skew);
      }
      default:
        return zero;
    }
  }

  /**
   * @brief Get the skew value for the given state.
   * @param _state The current state of the envelope.
   * @return The skew value.
   */
  [[nodiscard]] inline float getSkew(const State _state) const noexcept
  {
    switch (_state) {
      case State::Attack:
        return dmt::math::linearToExponent(params.attackSkew);
      case State::Decay:
        return dmt::math::linearToExponent(-params.decaySkew);
      default:
        return 1.0f;
    }
  }

  /**
   * @brief Get the sample index where the hold phase starts.
   * @return The sample index.
   */
  [[nodiscard]] inline size_t getHoldStart() const noexcept
  {
    return static_cast<size_t>(params.attack * sampleRate);
  }

  /**
   * @brief Get the sample index where the decay phase starts.
   * @return The sample index.
   */
  [[nodiscard]] inline size_t getDecayStart() const noexcept
  {
    const float rawDecayDelay = params.attack + params.hold;
    return static_cast<size_t>(rawDecayDelay * sampleRate) + 1;
  }

  /**
   * @brief Get the sample index where the decay phase ends.
   * @return The sample index.
   */
  [[nodiscard]] inline size_t getDecayEnd() const noexcept
  {
    const float rawDecayEnd = params.attack + params.hold + params.decay;
    return static_cast<size_t>(rawDecayEnd * sampleRate);
  }

  float sampleRate = -1.0f;
  Parameters params;
  size_t sampleIndex = 0;
};

//==============================================================================

} // namespace envelope
} // namespace dsp
} // namespace dmt

//==============================================================================
