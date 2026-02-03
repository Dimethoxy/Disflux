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
 * High-performance analog oscillator for real-time audio synthesis.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "AnalogWaveform.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace dsp {
namespace synth {

//==============================================================================

/**
 * @class AnalogOscillator
 * @brief High-performance analog oscillator for real-time audio synthesis.
 *
 * This class is designed for maximum real-time performance, using aggressive
 * optimizations such as constexpr, inline, noexcept, and forceinline. It
 * generates analog waveforms with various modulation capabilities.
 */
class alignas(64) AnalogOscillator
{
  using Math = juce::dsp::FastMathApproximations;
  static constexpr float twoPi = juce::MathConstants<float>::twoPi;
  static constexpr float pi = juce::MathConstants<float>::pi;

public:
  //==============================================================================
  /**
   * @brief Sets the sample rate for the oscillator.
   * @param _newSampleRate The new sample rate in Hz.
   */
  inline void setSampleRate(const float _newSampleRate) noexcept
  {
    TRACER("AnalogOscillator::setSampleRate");
    float rangeEnd =
      std::nextafter(392000.0f, std::numeric_limits<float>::max());
    const juce::Range<float> validRange(20.0f, rangeEnd);
    jassert(validRange.contains(_newSampleRate));
    sampleRate = _newSampleRate;
  }

  //==============================================================================
  /**
   * @brief Generates the next sample of the waveform.
   * @return The next sample value.
   */
  [[nodiscard]] forcedinline float getNextSample() noexcept
  {
    TRACER("AnalogOscillator::getNextSample");
    if (sampleRate <= 0.0f)
      return 0.0f;

    advancePhase();

    auto syncedPhase = getSyncedPhase(phase * pwmModifier);
    auto bendedPhase = getBendedPhase(syncedPhase);

    if (phase >= twoPi / pwmModifier)
      return 0.0f;

    float sample = waveform.getSample(bendedPhase);
    distortSample(sample);
    return std::clamp(sample, -1.0f, +1.0f);
  }

  //==============================================================================
  /**
   * @brief Sets the frequency of the oscillator.
   * @param _newFrequency The new frequency in Hz.
   */
  inline void setFrequency(const float _newFrequency) noexcept
  {
    TRACER("AnalogOscillator::setFrequency");
    frequency = _newFrequency;
  }

  //==============================================================================
  /**
   * @brief Sets the waveform type of the oscillator.
   * @param _type The new waveform type.
   */
  inline void setWaveformType(
    const dmt::dsp::synth::AnalogWaveform::Type _type) noexcept
  {
    TRACER("AnalogOscillator::setWaveformType");
    waveform.type = _type;
  }

  //==============================================================================
  /**
   * @brief Sets the drive level for waveform distortion.
   * @param _newDrive The new drive level.
   */
  inline void setDrive(const float _newDrive) noexcept { 
    TRACER("AnalogOscillator::setDrive");
    drive = _newDrive; 
  }

  //==============================================================================
  /**
   * @brief Sets the bias level for waveform distortion.
   * @param _newBias The new bias level.
   */
  inline void setBias(const float _newBias) noexcept { 
    TRACER("AnalogOscillator::setBias");
    bias = _newBias; 
  }

  //==============================================================================
  /**
   * @brief Sets the initial phase of the oscillator.
   * @param _newPhase The new phase value.
   */
  inline void setPhase(const float _newPhase) noexcept { 
    TRACER("AnalogOscillator::setPhase");
    phase = _newPhase; 
  }

  //==============================================================================
  /**
   * @brief Sets the bend modifier for waveform shaping.
   * @param _newBendModifier The new bend modifier value.
   */
  inline void setBend(const float _newBendModifier) noexcept
  {
    TRACER("AnalogOscillator::setBend");
    float rangeEnd = std::nextafter(100.0f, std::numeric_limits<float>::max());
    const juce::NormalisableRange<float> sourceRange(-100.0f, rangeEnd);
    jassert(sourceRange.getRange().contains(_newBendModifier));
    const auto normalisedValue = sourceRange.convertTo0to1(_newBendModifier);
    const juce::NormalisableRange<float> targetRange(0.1f, 0.9f);
    posityCycleRatio = targetRange.convertFrom0to1(normalisedValue);
  }

  //==============================================================================
  /**
   * @brief Sets the PWM (Pulse Width Modulation) modifier.
   * @param _newPwmModifier The new PWM modifier value.
   */
  inline void setPwm(const float _newPwmModifier) noexcept
  {
    TRACER("AnalogOscillator::setPwm");
    float rangeEnd = std::nextafter(100.0f, std::numeric_limits<float>::max());
    const juce::NormalisableRange<float> sourceRange(0.0f, rangeEnd);
    jassert(sourceRange.getRange().contains(_newPwmModifier));
    const auto normalisedValue = sourceRange.convertTo0to1(_newPwmModifier);
    const juce::NormalisableRange<float> targetRange(1.0f, 5.0f);
    pwmModifier = targetRange.convertFrom0to1(normalisedValue);
  }

  //==============================================================================
  /**
   * @brief Sets the sync modifier for phase synchronization.
   * @param _newSyncModifier The new sync modifier value.
   */
  inline void setSync(const float _newSyncModifier) noexcept
  {
    TRACER("AnalogOscillator::setSync");
    float rangeEnd = std::nextafter(100.0f, std::numeric_limits<float>::max());
    const juce::NormalisableRange<float> sourceRange(0.0f, rangeEnd);
    jassert(sourceRange.getRange().contains(_newSyncModifier));
    const auto normalisedValue = sourceRange.convertTo0to1(_newSyncModifier);
    const juce::NormalisableRange<float> targetRange(1.0f, 5.0f);
    syncModifier = targetRange.convertFrom0to1(normalisedValue);
  }

private:
  dmt::dsp::synth::AnalogWaveform waveform;
  float frequency = 50.0f;
  float sampleRate = -1.0f;
  float phase = 0.0f;

  float drive = 0.0f;
  float bias = 0.0f;
  float pwmModifier = 1.0f;
  float syncModifier = 1.0f;
  float posityCycleRatio = 0.5f;

  //==============================================================================
  /**
   * @brief Advances the phase of the oscillator.
   */
  forcedinline void advancePhase() noexcept
  {
    TRACER("AnalogOscillator::advancePhase");
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
    TRACER("AnalogOscillator::getSyncedPhase");
    float syncedPhase = _rawPhase * syncModifier;
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
    TRACER("AnalogOscillator::getBendedPhase");
    auto bendedPhase = _rawPhase;

    float positiveCycleSize = posityCycleRatio * twoPi;
    float negativeCycleRatio = 1.0f - posityCycleRatio;
    float negativeCycleSize = negativeCycleRatio * twoPi;

    if (_rawPhase <= positiveCycleSize) {
      bendedPhase /= (posityCycleRatio * 2.0f);
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
  forcedinline void distortSample(float& _sample) const noexcept
  {
    TRACER("AnalogOscillator::distortSample");
    constexpr float magicNumber = 0.7615941559558f;
    if (drive >= 1.0f) {
      _sample = Math::tanh(drive * _sample);
    } else {
      float invertedDrive = 1.0f - drive;
      float wetSample = drive * Math::tanh(_sample);
      float drySample = invertedDrive * _sample * magicNumber;
      _sample = wetSample + drySample;
    }

    _sample = _sample + bias;
  }
};

//==============================================================================
} // namespace synth
} // namespace dsp
} // namespace dmt
