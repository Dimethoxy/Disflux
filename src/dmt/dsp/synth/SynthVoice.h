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

#include "dsp/envelope/AdhEnvelope.h"
#include "dsp/synth/AnalogOscillator.h"
#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace dsp {
namespace synth {

//==============================================================================

/**
 * @class SynthVoice
 * @brief A class representing a synthesizer voice.
 */
class alignas(64) SynthVoice : public juce::SynthesiserVoice
{
public:
  //==============================================================================
  /**
   * @brief Constructor for SynthVoice.
   * @param _apvts Reference to the AudioProcessorValueTreeState.
   */
  SynthVoice(juce::AudioProcessorValueTreeState& _apvts) noexcept
    : apvts(_apvts)
  {
    TRACER("SynthVoice::SynthVoice");
  }

  //==============================================================================
  /**
   * @brief Determines if the voice can play a given sound.
   * @param _sound Pointer to the SynthesiserSound.
   * @return True if the sound can be played, false otherwise.
   */
  bool canPlaySound(juce::SynthesiserSound* _sound) override
  {
    TRACER("SynthVoice::canPlaySound");
    return dynamic_cast<juce::SynthesiserSound*>(_sound) != nullptr;
  }

  //==============================================================================
  /**
   * @brief Handles the event when a controller is moved.
   *
   * This function is called when a controller is moved, providing the
   * controller number and the new value of the controller. It is intended to be
   * overridden by derived classes to implement specific behavior for controller
   * movements.
   *
   * @param controllerNumber The number of the controller that was moved.
   * @param newControllerValue The new value of the controller.
   */
  void controllerMoved(int /*controllerNumber*/,
                       int /*newControllerValue*/) noexcept override
  {
    TRACER("SynthVoice::controllerMoved");
  }

  //==============================================================================
  /**
   * @brief Handles the event when the pitch wheel is moved.
   *
   * This function is called whenever the pitch wheel is moved. The new pitch
   * wheel value is passed as an argument, but it is currently unused.
   *
   * @param newPitchWheelValue The new value of the pitch wheel.
   */
  void pitchWheelMoved(int /*newPitchWheelValue*/) noexcept override {
    TRACER("SynthVoice::pitchWheelMoved");
  }

  //==============================================================================
  /**
   * @brief Prepares the voice to play.
   * @param _sampleRate The sample rate.
   * @param _samplesPerBlock Number of samples per block.
   * @param _outputChannels Number of output channels.
   */
  void prepareToPlay(double _sampleRate,
                     int /*_samplesPerBlock*/,
                     int /*_outputChannels*/) noexcept
  {
    TRACER("SynthVoice::prepareToPlay");
    if (_sampleRate <= 0)
      return;

    gainEnvelope.setSampleRate(static_cast<float>(_sampleRate));
    pitchEnvelope.setSampleRate(static_cast<float>(_sampleRate));
    osc.setSampleRate(static_cast<float>(_sampleRate));

    isPrepared = true;
  }

  //==============================================================================
  /**
   * @brief Starts a note.
   * @param _midiNoteNumber The MIDI note number.
   * @param _velocity The velocity of the note.
   * @param _sound Pointer to the SynthesiserSound.
   * @param _currentPitchWheelPosition The current pitch wheel position.
   */
  void startNote(int _midiNoteNumber,
                 float /*_velocity*/,
                 juce::SynthesiserSound* /*_sound*/,
                 int /*_currentPitchWheelPosition*/) noexcept override
  {
    TRACER("SynthVoice::startNote");
    osc.setPhase(0.0f);
    note = _midiNoteNumber;

    updateEnvelopeParameters();
    gainEnvelope.noteOn();
    pitchEnvelope.noteOn();

    callOnNoteReceivers();
  }

  //==============================================================================
  void stopNote(float /*_velocity*/, bool /*_allowTailOff*/) noexcept override
  {
    TRACER("SynthVoice::stopNote");
  }

  /**
   * @brief Renders the next block of audio.
   * @param _outputBuffer The output buffer.
   * @param _startSample The start sample index.
   * @param _numSamples The number of samples to render.
   */
  void renderNextBlock(juce::AudioBuffer<float>& _outputBuffer,
                       int _startSample,
                       int _numSamples) noexcept override
  {
    TRACER("SynthVoice::renderNextBlock");
    if (!isVoiceActive() || !isPrepared)
      return;

    updateEnvelopeParameters();
    updateOscillatorParameters();

    const float oscGain =
      apvts.getRawParameterValue("osc1DistortionPreGain")->load();
    const int oscOctave = apvts.getRawParameterValue("osc1VoiceOctave")->load();
    const int oscSemitone =
      apvts.getRawParameterValue("osc1VoiceSemitone")->load();
    const float oscModDepth =
      apvts.getRawParameterValue("osc1PitchEnvDepth")->load();

    const auto endSample = _numSamples + _startSample;
    auto* leftChannel = _outputBuffer.getWritePointer(0);
    auto* rightChannel = _outputBuffer.getWritePointer(1);

    for (int sample = _startSample; sample < endSample; ++sample) {
      osc.setFrequency(getNextFrequency(oscOctave, oscSemitone, oscModDepth));
      const auto rawSample = osc.getNextSample();
      const auto gainedSample = applyGain(rawSample, oscGain);
      leftChannel[sample] = gainedSample;
      rightChannel[sample] = gainedSample;
    }
  }

  //==============================================================================
  /**
   * @brief Adds a callback function to be called when a note is received.
   * @param _callbackFunction The callback function.
   */
  void addOnNoteReceivers(std::function<void()> _callbackFunction) noexcept
  {
    TRACER("SynthVoice::addOnNoteReceivers");
    onNoteReceivers.push_back(std::move(_callbackFunction));
  }

  //==============================================================================
  /**
   * @brief Calls all registered note receiver callback functions.
   */
  void callOnNoteReceivers() noexcept
  {
    TRACER("SynthVoice::callOnNoteReceivers");
    for (const auto& func : onNoteReceivers) {
      func();
    }
  }

protected:
  //==============================================================================
  /**
   * @brief Updates the envelope parameters from the
   * AudioProcessorValueTreeState.
   */
  void updateEnvelopeParameters() noexcept
  {
    TRACER("SynthVoice::updateEnvelopeParameters");
    dmt::dsp::envelope::AhdEnvelope::Parameters gainEnvParameters;
    gainEnvParameters.attack =
      apvts.getRawParameterValue("osc1GainEnvAttack")->load();
    gainEnvParameters.hold =
      apvts.getRawParameterValue("osc1GainEnvHold")->load();
    gainEnvParameters.decay =
      apvts.getRawParameterValue("osc1GainEnvDecay")->load();
    gainEnvParameters.decaySkew =
      apvts.getRawParameterValue("osc1GainEnvSkew")->load();
    gainEnvParameters.attackSkew = 0;
    gainEnvelope.setParameters(gainEnvParameters);

    dmt::dsp::envelope::AhdEnvelope::Parameters pitchEnvParameters;
    pitchEnvParameters.attack = 0;
    pitchEnvParameters.hold =
      apvts.getRawParameterValue("osc1PitchEnvHold")->load();
    pitchEnvParameters.decay =
      apvts.getRawParameterValue("osc1PitchEnvDecay")->load();
    pitchEnvParameters.decaySkew =
      apvts.getRawParameterValue("osc1PitchEnvSkew")->load();
    pitchEnvParameters.attackSkew = 0;
    pitchEnvelope.setParameters(pitchEnvParameters);
  }

  //==============================================================================
  /**
   * @brief Updates the oscillator parameters from the
   * AudioProcessorValueTreeState.
   */
  void updateOscillatorParameters() noexcept
  {
    TRACER("SynthVoice::updateOscillatorParameters");
    osc.setWaveformType(static_cast<dmt::dsp::synth::AnalogWaveform::Type>(
      apvts.getRawParameterValue("osc1WaveformType")->load()));
    osc.setDrive(apvts.getRawParameterValue("osc1DistortionType")->load());
    osc.setBias(apvts.getRawParameterValue("osc1DistortionSymmetry")->load());
    osc.setBend(apvts.getRawParameterValue("osc1WaveformBend")->load());
    osc.setPwm(apvts.getRawParameterValue("osc1WaveformPwm")->load());
    osc.setSync(apvts.getRawParameterValue("osc1WaveformSync")->load());
  }

  //==============================================================================
  /**
   * @brief Calculates the next frequency for the oscillator.
   * @param _rawOctave The raw octave value.
   * @param _rawSemitone The raw semitone value.
   * @param _rawModDepth The raw modulation depth.
   * @return The next frequency.
   */
  [[nodiscard]] float getNextFrequency(const int _rawOctave,
                                       const int _rawSemitone,
                                       const float _rawModDepth) noexcept
  {
    TRACER("SynthVoice::getNextFrequency");
    const int octaves = 12 * _rawOctave;
    const int semitone = octaves + _rawSemitone;
    const int baseNote = note + semitone;
    const float baseFreq = juce::MidiMessage::getMidiNoteInHertz(baseNote);
    const float modDepth = _rawModDepth * 2e4f;
    const float envelopeSample = pitchEnvelope.getNextSample();
    const float maxFreq = std::clamp(baseFreq + modDepth, baseFreq, 2e4f);
    const float newFreq = juce::mapToLog10(envelopeSample, baseFreq, maxFreq);
    return std::clamp(newFreq, 20.0f, 2e4f);
  }

  //==============================================================================
  /**
   * @brief Applies gain to a sample.
   * @param _sample The input sample.
   * @param _oscGain The oscillator gain.
   * @return The gained sample.
   */
  [[nodiscard]] float applyGain(float _sample, float _oscGain) noexcept
  {
    TRACER("SynthVoice::applyGain");
    const float envGain = gainEnvelope.getNextSample();
    const float gain = juce::Decibels::decibelsToGain(_oscGain, -96.0f);
    return _sample * envGain * gain;
  }

private:
  juce::AudioProcessorValueTreeState& apvts;
  dmt::dsp::synth::AnalogOscillator osc;
  dmt::dsp::envelope::AhdEnvelope gainEnvelope;
  dmt::dsp::envelope::AhdEnvelope pitchEnvelope;
  int note = 0;
  bool isPrepared = false;
  std::vector<std::function<void()>> onNoteReceivers;
};

//==============================================================================

} // namespace synth
} // namespace dsp
} // namespace dmt

//==============================================================================
