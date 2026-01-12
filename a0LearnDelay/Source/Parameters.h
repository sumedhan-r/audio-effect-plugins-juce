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
  /*
   Below is a macro that is used for two purposes

      1) No copy constructor and copy assignment operator is used
         by this class, since having a copy of the plug-in class
         object is not desirable for us
      2) Leak detector ensures memory deallocation happens for
         out-of-scope instance without specific intervention
  */
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)

  juce::AudioParameterFloat *gainParam;
  juce::LinearSmoothedValue<float> gainSmoother;

  juce::AudioParameterFloat *delayTimeParam;

  juce::AudioParameterFloat *mixParam;
  juce::LinearSmoothedValue<float> mixSmoother;
};
