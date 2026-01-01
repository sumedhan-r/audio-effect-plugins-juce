/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
_01_Learn_DelayAudioProcessorEditor::_01_Learn_DelayAudioProcessorEditor(
    _01_Learn_DelayAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(400, 300);
}

_01_Learn_DelayAudioProcessorEditor::~_01_Learn_DelayAudioProcessorEditor() {}

//==============================================================================
void _01_Learn_DelayAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(juce::Colours::blueviolet); // change_1

  g.setColour(juce::Colours::white);
  g.setFont(juce::FontOptions(40.0f)); // change_2
  g.drawFittedText("My first plug-in", getLocalBounds(),
                   juce::Justification::centred, 1); // change_3
}

void _01_Learn_DelayAudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
}
