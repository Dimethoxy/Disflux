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
 * Neutrino Processor class for processing audio buffers to generate a kick drum
 * sound.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include "dsp/synth/NeutrinoSynthVoice.h"
#include "dsp/synth/SynthSound.h"
#include <JuceHeader.h>
#include <utility/Settings.h>

namespace dmt {
namespace dsp {
namespace effect {

//==============================================================================

/**
 * @brief Neutrino Processor
 *
 * This class processes audio buffers to generate a kick drum sound.
 */
class alignas(64) NeutrinoProcessor
{
  constexpr static float MIN_FREQUENCY = 20.0f;
  constexpr static float MAX_FREQUENCY = 20000.0f;

  using AudioBuffer = juce::AudioBuffer<float>;
  using SynthVoice = dmt::dsp::synth::NeutrinoSynthVoice;
  using SynthSound = dmt::dsp::synth::SynthSound;

public:
  NeutrinoProcessor(juce::AudioProcessorValueTreeState& _apvts)
    : apvts(_apvts)
  {
    synth.addSound(new SynthSound());
    synth.addVoice(new SynthVoice(apvts));
  }

  //==============================================================================
  /**
   * @brief Prepares the processor with the given sample rate.
   *
   * @param _newSampleRate The sample rate.
   */
  inline void prepare(const double _newSampleRate,
                      const int _samplesPerBlock) noexcept
  {
    sampleRate = static_cast<float>(_newSampleRate);

    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); i++) {
      auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i));
      voice->prepareToPlay(sampleRate, _samplesPerBlock, 2);
    }
  }

  inline void processBlock(AudioBuffer& _buffer,
                           juce::MidiBuffer& _midiMessages) noexcept
  {
    if (sampleRate <= 0.0f) {
      return;
    }

    synth.renderNextBlock(_buffer, _midiMessages, 0, _buffer.getNumSamples());
  }

private:
  //==============================================================================
  juce::Synthesiser synth;
  juce::AudioProcessorValueTreeState& apvts;
  float sampleRate = -1.0f;
};

//==============================================================================
} // namespace effect
} // namespace dsp
} // namespace dmt