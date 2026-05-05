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
 * High-performance digital oscillator for real-time audio synthesis.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dsp/synth/DigitalWaveform.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace dsp {
namespace synth {
//==============================================================================

/**
 * @class DigitalOscillator
 * @brief High-performance digital oscillator for real-time audio synthesis.
 *
 * This class is designed for maximum real-time performance, using aggressive
 * optimizations such as constexpr, inline, noexcept, and forceinline. It
 * generates digital waveforms with various modulation capabilities.
 */
class alignas(64) DigitalOscillator
{
  using Math = juce::dsp::FastMathApproximations;
  using String = juce::String;
  using DigitalWaveform = dmt::dsp::synth::DigitalWaveform;

  static constexpr float twoPi = juce::MathConstants<float>::twoPi;
  static constexpr float halfPi = juce::MathConstants<float>::halfPi;
  static constexpr float pi = juce::MathConstants<float>::pi;

public:
  struct Parameters
  {
  public:
    DigitalWaveform::Type type = DigitalWaveform::Type::Sine;
    float bias = 0.0f;
    float drive = 0.0f;
    float pwm = 0.0f;
    float clip = 0.0f;
    float warp = 0.0f;

  private:
    float sync = 0.0f;
    float bend = 0.0f;

  public:
    //==============================================================================
    inline float getBend() const noexcept { return bend; }
    inline void setBend(const float _newBend) noexcept
    {
      TRACER("DigitalOscillator::setBend");
      float rangeEnd =
        std::nextafter(100.0f, std::numeric_limits<float>::max());
      const juce::NormalisableRange<float> sourceRange(-100.0f, rangeEnd);
      jassert(sourceRange.getRange().contains(_newBend));
      const auto normalisedValue = sourceRange.convertTo0to1(_newBend);
      const juce::NormalisableRange<float> targetRange(0.1f, 0.9f);
      bend = targetRange.convertFrom0to1(normalisedValue);
    }

    //==============================================================================
    inline float getSync() const noexcept { return sync; }
    inline void setSync(const float _newSync) noexcept
    {
      TRACER("DigitalOscillator::setSync");
      float rangeEnd =
        std::nextafter(100.0f, std::numeric_limits<float>::max());
      const juce::NormalisableRange<float> sourceRange(0.0f, rangeEnd);
      jassert(sourceRange.getRange().contains(_newSync));
      const auto normalisedValue = sourceRange.convertTo0to1(_newSync);
      const juce::NormalisableRange<float> targetRange(1.0f, 5.0f);
      sync = targetRange.convertFrom0to1(normalisedValue);
    }
  };

  //==============================================================================
  // Oscillator
public:
  inline void setParameters(const juce::AudioProcessorValueTreeState& apvts,
                            String prefix)
  {
    auto type = params.type;
    float warp = params.warp;
    float bend = params.getBend();
    float pwm = params.pwm;
    float sync = params.getSync();
    float bias = params.bias;
    float clip = params.clip;
    float drive = params.drive;

    String base = prefix + "DigitalOscillator";
    type = static_cast<DigitalWaveform::Type>(
      apvts.getRawParameterValue(base + "Type")->load());
    bend = apvts.getRawParameterValue(base + "Bend")->load();
    warp = apvts.getRawParameterValue(base + "Warp")->load();
    pwm = apvts.getRawParameterValue(base + "Pwm")->load();
    sync = apvts.getRawParameterValue(base + "Sync")->load();
    bias = apvts.getRawParameterValue(base + "Bias")->load();
    clip = apvts.getRawParameterValue(base + "Clip")->load();
    drive = apvts.getRawParameterValue(base + "Drive")->load();

    // These don't need mapping so we can set them directly
    waveform.type = type;
    params.pwm = pwm;
    params.bias = bias;
    params.clip = clip;
    params.drive = drive;
    params.warp = warp;

    // These need mapping so we use setters to do that
    params.setBend(bend);
    params.setSync(sync);

    // Eagerly compute PWM end sample
    computePwmEndSample();
  }

  //==============================================================================
  /**
   * @brief Sets the sample rate for the oscillator.
   * @param _newSampleRate The new sample rate in Hz.
   */
  inline void setSampleRate(const float _newSampleRate) noexcept
  {
    TRACER("DigitalOscillator::setSampleRate");
    sampleRate = _newSampleRate;
    computePwmEndSample();
  }

  //==============================================================================
  /**
   * @brief Generates the next sample of the waveform.
   * @return The next sample value.
   */
  [[nodiscard]] forcedinline float getNextSample() noexcept
  {
    TRACER("DigitalOscillator::getNextSample");

    using std::pow, std::clamp;

    if (sampleRate <= 0.0f)
      return 0.0f;

    advancePhase();

    auto warpedPhase = getWarpPhase(phase);
    auto syncedPhase = getSyncedPhase(warpedPhase);
    auto bendedPhase = getBendedPhase(syncedPhase);

    auto pwmPhase = bendedPhase;
    auto pwm = params.pwm;
    const float pwmNormalized = 1.0f - (pwm / 100.0f); // 0.8

    if (pwmPhase >= twoPi * pwmNormalized) {
      return pwmEndSample;
    }

    if (pwmNormalized > 0.0f || pwmNormalized < 1.0f) {
      pwmPhase = clamp(pwmPhase / pwmNormalized, 0.0f, twoPi);
    }

    float sample = waveform.getSample(pwmPhase);
    sample = distortSample(sample);
    return clamp(sample, -1.0f, +1.0f);
  }

  //==============================================================================
  // Experimental phase warp function
  float getWarpPhase(float _phase) const noexcept
  {
    const float k = params.warp;
    const float normalizedPhase = _phase / pi; // Normalize phase to [0, 2]

    auto b = [k](float x) { return (1.0 + 2.0 * k) * x; };
    auto c = [k](float x) { return (1.0 - 2.0 * k) * x + 2.0 * k; };
    auto d = [k](float x) { return (1.0 + 2.0 * k) * x - 4.0 * k; };

    if (normalizedPhase < 0.5) {
      return b(normalizedPhase) * pi;
    }

    if (normalizedPhase < 1.5) {
      return c(normalizedPhase) * pi;
    }

    return d(normalizedPhase) * pi;
  }

  //==============================================================================
  void reset() noexcept
  {
    TRACER("DigitalOscillator::reset");
    phase = 0.0f;
    computePwmEndSample();
  }

  //==============================================================================
  /**
   * @brief Sets the frequency of the oscillator.
   * @param _newFrequency The new frequency in Hz.
   */
  inline void setFrequency(const float _newFrequency) noexcept
  {
    TRACER("DigitalOscillator::setFrequency");
    frequency = _newFrequency;
  }

  //==============================================================================
  /**
   * @brief Advances the phase of the oscillator.
   */
  forcedinline void advancePhase() noexcept
  {
    TRACER("DigitalOscillator::advancePhase");
    float cycleLength = sampleRate / frequency;
    float phaseDelta = twoPi / cycleLength;
    phase += phaseDelta;

    if (phase >= twoPi) {
      phase -= twoPi;
    }
  }

  //==============================================================================
  /**
   * @brief Computes the synced phase based on the raw phase and sync modifier.
   * @param _rawPhase The raw phase value.
   * @return The synced phase value.
   */
  forcedinline float getSyncedPhase(float _rawPhase) const noexcept
  {
    TRACER("DigitalOscillator::getSyncedPhase");
    float syncedPhase = _rawPhase * params.getSync();
    while (syncedPhase >= twoPi) {
      syncedPhase -= twoPi;
    }
    return syncedPhase;
  }

  //==============================================================================
  /**
   * @brief Computes the bended phase based on the raw phase and bend modifier.
   * @param _rawPhase The raw phase value.
   * @return The bended phase value.
   */
  forcedinline float getBendedPhase(float _rawPhase) const noexcept
  {
    TRACER("DigitalOscillator::getBendedPhase");
    auto bendedPhase = _rawPhase;

    const float bend = params.getBend();
    float positiveCycleSize = bend * twoPi;
    float negativeCycleRatio = 1.0f - bend;
    float negativeCycleSize = negativeCycleRatio * twoPi;

    if (_rawPhase <= positiveCycleSize) {
      bendedPhase /= (bend * 2.0f);
    }

    if (_rawPhase > positiveCycleSize) {
      bendedPhase = (_rawPhase - positiveCycleSize) / negativeCycleSize;
      bendedPhase = bendedPhase * pi + pi;
    }
    return bendedPhase;
  }

  //==============================================================================
  /**
   * @brief Applies distortion to the sample based on the drive and bias
   * settings.
   * @param _sample The sample value to be distorted.
   */
  forcedinline float distortSample(float _sample) const noexcept
  {
    TRACER("DigitalOscillator::distortSample");

    using std::clamp, std::abs, std::atan, std::tan;

    float drive = params.drive;
    float bias = params.bias;
    float clip = params.clip + 1.0f;
    float k = abs(drive);

    float biasSample = _sample + bias;
    float clipSample = clamp(biasSample * clip, -1.0f, 1.0f);

    if (k < 0.01f)
      return clipSample;

    if (drive > 0.0f) {
      float normalizer = atan(k);
      return atan(k * clipSample) / normalizer;
    } else {
      float normalizer = atan(k * 0.2f);
      return tan(clipSample * normalizer) / (k * 0.2f);
    }
  }

private:
  //==============================================================================
  /**
   * @brief Computes the PWM end-of-cycle sample.
   */
  void computePwmEndSample() noexcept
  {
    TRACER("DigitalOscillator::computePwmEndSample");

    // We sample the waveform at 2π to get it's last sample and use it as PWM
    // fill
    float endPhase = getBendedPhase(getSyncedPhase(twoPi));
    pwmEndSample = waveform.getSample(endPhase);
    distortSample(pwmEndSample);
    pwmEndSample = std::clamp(pwmEndSample, -1.0f, +1.0f);
  }

  //==============================================================================
  DigitalWaveform waveform;
  Parameters params;
  float frequency = 50.0f;
  float sampleRate = -1.0f;
  float phase = 0.0f;
  float pwmEndSample = 0.0f;
  //==============================================================================
};
} // namespace synth
} // namespace dsp
} // namespace dmt
