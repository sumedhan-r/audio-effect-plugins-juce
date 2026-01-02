/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
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
};
