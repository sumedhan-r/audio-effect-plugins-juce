/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 2 Jan 2026 8:18:01pm
    Author:  Sumedhan Ramesh

  ==============================================================================
*/

#include "RotaryKnob.h"
#include "LookAndFeel.h"
#include <JuceHeader.h>

//==============================================================================
RotaryKnob::RotaryKnob(const juce::String &text,
                       juce::AudioProcessorValueTreeState &apvts,
                       const juce::ParameterID &parameterID,
                       bool drawFromMiddle)
    : attachment(apvts, parameterID.getParamID(), slider) {
  // In your constructor, you should add any child components, and
  // initialise any special settings that your component needs.

  float pi = juce::MathConstants<float>::pi;
  slider.setRotaryParameters(1.25f * pi, 2.75f * pi, true);
  slider.setSliderStyle(
      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
  // text (number) box position, is read, text box width, text box height
  slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 16);
  slider.setBounds(0, 0, 70, 86); // 86 = 70 (knob height) + 16 (box height)
  slider.getProperties().set("drawFromMiddle", drawFromMiddle);
  addAndMakeVisible(slider);

  label.setText(text, juce::NotificationType::dontSendNotification);
  label.setJustificationType(juce::Justification::horizontallyCentred);
  label.setBorderSize(juce::BorderSize<int>(0));
  label.attachToComponent(&slider, false);
  addAndMakeVisible(label);

  setLookAndFeel(RotaryKnobLookAndFeel::get());

  setSize(70, 110); // 110 = 86 (full slider height) + 24 (label height)
}

RotaryKnob::~RotaryKnob() {}

void RotaryKnob::resized() {
  // This method is where you should set the bounds of any child
  // components that your component contains..
  slider.setTopLeftPosition(0, 24);
}
