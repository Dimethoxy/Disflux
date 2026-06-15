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
 * You must adhere GPLv3 license for any project and parts of it.
 * You are not allowed to use this code in any closed-source project.
 *
 * Description:
 * ImpulseResponseBuilder generates an impulse response by injecting a unit
 * impulse into a processor and capturing the output. It tracks parameter
 * changes via an APVTS reference and a list of parameter IDs to watch,
 * marking the IR as dirty when any watched parameter changes.
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
namespace gui {
namespace display {

//==============================================================================

/**
 * @brief Generates and caches impulse responses from any processor.
 *
 * Takes a processor in the constructor, allocates an internal buffer of the
 * given impulse size, and provides rebuild() to clear the buffer, inject a
 * unit impulse at sample 0, and run it through the processor's processBlock.
 *
 * Parameter change tracking is done by registering a listener against the
 * APVTS for each parameter ID in the watch list. When any watched parameter
 * changes, the IR is marked dirty so the next rebuild() will regenerate it.
 */
template<typename ProcessorType>
class ImpulseResponseBuilder : public juce::AudioProcessorParameter::Listener
{
  using AudioBuffer = juce::AudioBuffer<float>;

public:
  //==============================================================================
  /**
   * @brief Constructs the builder with the given processor and impulse size.
   *
   * @param _processor Reference to the processor to run the impulse through.
   * @param _apvts Reference to the APVTS whose parameter changes we watch.
   * @param _impulseSize Number of samples in the impulse response.
   * @param _parametersToWatch List of parameter IDs to listen for changes.
   */
  ImpulseResponseBuilder(ProcessorType& _processor,
                         juce::AudioProcessorValueTreeState& _apvts,
                         int _impulseSize,
                         juce::Array<juce::Identifier> _parametersToWatch)
    : processor(_processor)
    , apvts(_apvts)
  {
    buffer.setSize(2, _impulseSize);
    buffer.clear();

    // Register listeners for each watched parameter
    for (const auto& paramId : _parametersToWatch) {
      auto* param = apvts.getParameter(paramId.toString());
      if (param) {
        param->addListener(this);
        watchedParams.add(paramId);
      }
    }

    // Build the initial IR
    rebuild();
  }

  ~ImpulseResponseBuilder()
  {
    // Remove all listeners
    for (const auto& paramId : watchedParams) {
      auto* param = apvts.getParameter(paramId.toString());
      if (param) {
        param->removeListener(this);
      }
    }
  }

  //==============================================================================
  /**
   * @brief Rebuilds the impulse response by injecting a unit impulse and
   *        processing it through the processor.
   *
   * This is called both initially and whenever the IR is marked dirty.
   */
  void rebuild()
  {
    if (isDirty || buffer.getNumSamples() == 0) {
      // Clear buffer and inject unit impulse at sample 0, both channels
      buffer.clear();
      buffer.setSample(0, 0, 1.0f);
      buffer.setSample(1, 0, 1.0f);

      // Process through the processor
      processor.processBlock(buffer);

      isDirty = false;
    }
  }

  //==============================================================================
  /**
   * @brief Returns true if any watched parameter has changed since rebuild().
   */
  bool isDirtyFlag() const noexcept { return isDirty; }

  //==============================================================================
  /**
   * @brief Returns a reference to the internal impulse response buffer.
   */
  AudioBuffer& getBuffer() noexcept { return buffer; }

  const AudioBuffer& getBuffer() const noexcept { return buffer; }

  //==============================================================================
  /**
   * @brief Marks the IR as dirty so it will be regenerated on next rebuild().
   */
  void markDirty() noexcept { isDirty = true; }

  //==========================================================================
  // Parameter listener implementation

  void parameterChanged(const String& _parameterID, float _newValue)
  {
    // Check if the changed parameter is in our watch list
    if (watchedParams.contains(_parameterID)) {
      markDirty();
    }
  }

private:
  //==============================================================================
  ProcessorType& processor;
  juce::AudioProcessorValueTreeState& apvts;
  AudioBuffer buffer;
  bool isDirty = false;
  juce::Array<juce::Identifier> watchedParams;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImpulseResponseBuilder)
};

//==============================================================================
} // namespace display
} // namespace gui
} // namespace dmt

//==============================================================================