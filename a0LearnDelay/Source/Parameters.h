/*
  ==============================================================================

    Parameters.h
    Created: 2 Jan 2026 2:59:05pm
    Author:  Sumedhan Ramesh

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const juce::ParameterID gainParamID{"gain", 1};
const juce::ParameterID delayTimeParamID{"delayTime", 1};
const juce::ParameterID mixParamID{"mix", 1};

class Parameters {
public:
  Parameters(juce::AudioProcessorValueTreeState &apvts);

  static juce::AudioProcessorValueTreeState::ParameterLayout
  createParameterLayout();

  void prepareToPlay(double sampleRate) noexcept;
  void reset() noexcept;
  void update() noexcept;
  void smoothen() noexcept;

  float gain = 0.0f;

  static constexpr float minDelayTime = 5.0f;
  static constexpr float maxDelayTime = 5000.0f;

  float delayTime = 0.0f;

  // One-pole filter smoothing for delay
  float targetDelayTime = 0.0f;
  float coeff = 0.0f;

  // Mix for dry and wet samples from delay line
  float mix = 1.0f;

private:
  juce::AudioParameterFloat *gainParam;
  juce::LinearSmoothedValue<float> gainSmoother;

  juce::AudioParameterFloat *delayTimeParam;

  juce::AudioParameterFloat *mixParam;
  juce::LinearSmoothedValue<float> mixSmoother;
};
