/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "LookAndFeel.h"
#include "Parameters.h"
#include "PluginProcessor.h"
#include "RotaryKnob.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class A0LearnDelayAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  A0LearnDelayAudioProcessorEditor(A0LearnDelayAudioProcessor &);
  ~A0LearnDelayAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  A0LearnDelayAudioProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(A0LearnDelayAudioProcessorEditor)

  // Grouping the rotary knobs in the UI for better distinction of
  // each plug-in feature/slider/knob built.
  juce::GroupComponent delayGroup, feedbackGroup, outputGroup;

  // Rotary knob is a slider object under JUCE
  RotaryKnob gainKnob{"Gain", audioProcessor.apvts, gainParamID, true};
  RotaryKnob mixKnob{"Mix", audioProcessor.apvts, mixParamID};
  RotaryKnob delayTimeKnob{"Delay Time", audioProcessor.apvts,
                           delayTimeParamID};

  MainLookAndFeel mainLF;
};
