/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
A0LearnDelayAudioProcessorEditor::A0LearnDelayAudioProcessorEditor(
    A0LearnDelayAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.

  delayGroup.setText("Delay");
  delayGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
  delayGroup.addAndMakeVisible(delayTimeKnob);
  addAndMakeVisible(delayGroup);

  feedbackGroup.setText("Feedback");
  feedbackGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
  addAndMakeVisible(feedbackGroup);

  outputGroup.setText("Output");
  outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
  outputGroup.addAndMakeVisible(mixKnob);
  outputGroup.addAndMakeVisible(gainKnob);
  addAndMakeVisible(outputGroup);

  setLookAndFeel(&mainLF);

  setSize(500, 330);
}

A0LearnDelayAudioProcessorEditor::~A0LearnDelayAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

//==============================================================================
void A0LearnDelayAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour) g.fillAll(Colors::background);
  auto noise = juce::ImageCache::getFromMemory(BinaryData::Noise_png,
                                               BinaryData::Noise_pngSize);
  auto fillType = juce::FillType(noise, juce::AffineTransform::scale(0.5f));
  g.setFillType(fillType);
  g.fillRect(getLocalBounds());

  auto rect = getBounds().withHeight(40);
  g.setColour(Colors::header);
  g.fillRect(rect);

  auto image = juce::ImageCache::getFromMemory(BinaryData::Logo_png,
                                               BinaryData::Logo_pngSize);

  // Image is actually twice as big as needed in the UI
  int destWidth = image.getWidth() / 2;
  int destHeight = image.getHeight() / 2;
  // First 4 arguments - Destination x, y, width, height
  // Last four arguments - Source x, y, width, height
  g.drawImage(image, getWidth() / 2 - destWidth / 2, 0, destWidth, destHeight,
              0, 0, image.getWidth(), image.getHeight());
}

void A0LearnDelayAudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..

  auto bounds = getLocalBounds();

  int y = 10 + 40; // Top line of rectangle, 40 is for logo image iwdth
  int height = bounds.getHeight() - 20 -
               40; // Bottom line of rectangle, 40 is for logo image iwdth

  delayGroup.setBounds(10, y, 110, height);
  outputGroup.setBounds(bounds.getRight() - 160, y, 150, height);
  feedbackGroup.setBounds(delayGroup.getRight() + 10, y,
                          outputGroup.getX() - delayGroup.getRight() - 20,
                          height);

  delayTimeKnob.setTopLeftPosition(20, 20);
  mixKnob.setTopLeftPosition(20, 20);
  gainKnob.setTopLeftPosition(mixKnob.getX(), mixKnob.getBottom() + 10);
}
