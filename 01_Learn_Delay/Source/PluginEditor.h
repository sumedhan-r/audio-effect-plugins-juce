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
class _01_Learn_DelayAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  _01_Learn_DelayAudioProcessorEditor(_01_Learn_DelayAudioProcessor &);
  ~_01_Learn_DelayAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  _01_Learn_DelayAudioProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      _01_Learn_DelayAudioProcessorEditor)
};
