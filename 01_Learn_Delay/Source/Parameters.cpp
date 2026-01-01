/*
  ==============================================================================

    Parameters.cpp
    Created: 1 Jan 2026 7:14:54pm
    Author:  Sumedhan Ramesh

  ==============================================================================
*/

#include "Parameters.h"

template <typename T>
static void castParameter(juce::AudioProcessorValueTreeState &apvts,
                          const juce::ParameterID &id, T &destination) {
  destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
  jassert(destination);
}

Parameters::Parameters(juce::AudioProcessorValueTreeState &apvts) {
  castParameter(apvts, gainParamID, gainParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout
Parameters::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  layout.add(std::make_unique<juce::AudioParameterFloat>(
      gainParamID, "Output Gain", juce::NormalisableRange<float>{-12.0f, 12.0f},
      0.0f));

  return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept {
  /*
   The ramp length value is crucial here!!
      - Too short : The wave amplitude jumps with sudden spikes,
                    creating the block like progression and
                    the zipper noise persists
      - Too long : The wave sample gets too smoothened to the
                   point where one can hear the fade in and
                   fade out.

   Typical sample rate = 48kHz,
   Compromise = 0.02 * 48k = 960 samples.
  */
  double rampLengthInSeconds = 0.02;
  gainSmoother.reset(sampleRate, rampLengthInSeconds);
}

void Parameters::reset() noexcept {
  gain = 0.0f;

  gainSmoother.setCurrentAndTargetValue(
      juce::Decibels::decibelsToGain(gainParam->get()));
}

void Parameters::update() noexcept {
  gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
}

void Parameters::smoothen() noexcept { gain = gainSmoother.getNextValue(); }
