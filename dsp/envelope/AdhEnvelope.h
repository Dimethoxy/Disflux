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
    bool enabled = true;
    float attack = 0.015f;
    float hold = 0.08f;
    float decay = 0.5f;
    float attackBend = 0;
    float decayBend = 0;
    float depth = 1.0f;
  };

  enum class State
  {
    Disabled,
    Attack,
    Hold,
    Decay,
    Idle
  };

  constexpr AhdEnvelope() noexcept = default;

  inline void setParameters(const juce::AudioProcessorValueTreeState& apvts,
                            juce::String prefix) noexcept
  {
    juce::String base = prefix + "Env";
    params.enabled =
      apvts.getRawParameterValue(base + "Enabled")->load() > 0.5f;
    params.attack = apvts.getRawParameterValue(base + "Attack")->load();
    params.hold = apvts.getRawParameterValue(base + "Hold")->load();
    params.decay = apvts.getRawParameterValue(base + "Decay")->load();
    params.attackBend = apvts.getRawParameterValue(base + "AttackBend")->load();
    params.decayBend = apvts.getRawParameterValue(base + "DecayBend")->load();
    params.depth = apvts.getRawParameterValue(base + "Depth")->load();
  }

  /**
   * @brief Set the envelope parameters.
   * @param _newParams The new parameters to set.
   */
  inline void setParameters(const Parameters& _newParams) noexcept
  {
    params = _newParams;
  }

  /**
   * @brief Get the Parameters object
   *
   * @return The current Parameters object
   */
  inline Parameters getParameters() const noexcept { return params; }

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
    if (!params.enabled)
      return State::Disabled;
    if (sampleIndex < getHoldStart())
      return State::Attack;
    if (sampleIndex < getDecayStart())
      return State::Hold;
    if (sampleIndex < getDecayEnd())
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
      case State::Disabled:
        return 0.0f;
      case State::Attack: {
        const float normalizedPosition =
          static_cast<float>(sampleIndex) / sampleRate;
        const float phaseProgress = normalizedPosition / params.attack;
        return applyAtanBend(phaseProgress, params.attackBend);
      }
      case State::Hold:
        return one;
      case State::Decay: {
        const float decayStart = static_cast<float>(getDecayStart());
        const float normalizedPosition =
          (static_cast<float>(sampleIndex) - decayStart) / sampleRate;
        const float phaseProgress = normalizedPosition / params.decay;
        return one - applyAtanBend(phaseProgress, params.decayBend);
      }
      default:
        return zero;
    }
  }

  /**
   * @brief Apply atan-based bend to normalized phase in range [0, 1].
   */
  [[nodiscard]] static inline float applyAtanBend(float normalizedPhase,
                                                  float bend) noexcept
  {
    using juce::jlimit;
    using std::atan, std::pow, std::abs;

    // We make the bend curve more exponential to make it feel linear
    const float k = pow(0.5f * abs(bend), 2.0f);
    const float x = jlimit(0.0f, 1.0f, normalizedPhase);
    const float normalizer = atan(k);

    // No bend
    if (k <= 0.01f) [[unlikely]]
      return x;

    // Positive bend
    if (bend > 0.0f)
      return atan(k * x) / normalizer;

    // Negative bend
    return 1.0f - (atan(k * (1.0f - x)) / normalizer);
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
