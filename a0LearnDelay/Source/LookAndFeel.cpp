/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 4 Jan 2026 11:41:27pm
    Author:  Sumedhan Ramesh

  ==============================================================================
*/

#include "LookAndFeel.h"

const juce::Typeface::Ptr Fonts::typeface =
    juce::Typeface::createSystemTypefaceFor(BinaryData::LatoMedium_ttf,
                                            BinaryData::LatoMedium_ttfSize);

juce::Font Fonts::getFont(float height) {
  return juce::FontOptions(Fonts::typeface)
      .withHeight(height)
      .withMetricsKind(juce::TypefaceMetricsKind::legacy);
}

RotaryKnobLookAndFeel::RotaryKnobLookAndFeel() {
  setColour(juce::Label::textColourId, Colors::Knob::label);
  setColour(juce::Slider::textBoxTextColourId, Colors::Knob::label);
  setColour(juce::Slider::rotarySliderFillColourId, Colors::Knob::trackActive);
  // Colour ID is supposed to be Label outlieColourId, but this is replaced with
  // that of the Slider one all based on digging through module code base
  setColour(juce::Slider::textBoxOutlineColourId,
            juce::Colours::transparentBlack);
  setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
}

void RotaryKnobLookAndFeel::drawRotarySlider(
    juce::Graphics &g, int x, int y, int width, [[maybe_unused]] int height,
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider &slider) {
  auto bounds = juce::Rectangle<int>(x, y, width, width).toFloat();
  auto knobRect = bounds.reduced(10.0f, 10.0f);

  // Adding drop shadow for 3D look
  auto path = juce::Path();
  path.addEllipse(knobRect);
  dropShadow.drawForPath(g, path);

  g.setColour(Colors::Knob::outline);
  g.fillEllipse(knobRect);

  // Adding gradient circular outline
  auto innerRect = knobRect.reduced(2.0f, 2.0f);
  auto gradient = juce::ColourGradient(
      Colors::Knob::gradientTop, 0.0f, innerRect.getY(),
      Colors::Knob::gradientBottom, 0.0f, innerRect.getBottom(), false);
  g.setGradientFill(gradient);
  g.fillEllipse(innerRect);

  // Drawing an arc outside the knob to show start and end angles
  auto centre = bounds.getCentre();
  auto radius = bounds.getWidth() / 2.0f;
  auto lineWidth = 3.0f;
  auto arcRadius = radius - lineWidth / 2.0f;

  juce::Path backgroundArc;
  backgroundArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                              rotaryStartAngle, rotaryEndAngle, true);

  auto strokeType = juce::PathStrokeType(
      lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

  g.setColour(Colors::Knob::trackBackground);
  g.strokePath(backgroundArc, strokeType);

  // Knob radius line for pointing to current value
  auto dialRadius = innerRect.getHeight() / 2.0f - lineWidth;
  auto toAngle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  float pi = juce::MathConstants<float>::pi;

  // Start point of the knob radius is away from centre for better view
  juce::Point<float> dialStart(centre.x + 10.0f * std::cos(toAngle - pi / 2.0f),
                               centre.y +
                                   10.0f * std::sin(toAngle - pi / 2.0f));
  juce::Point<float> dialEnd(
      centre.x + dialRadius * std::cos(toAngle - pi / 2.0f),
      centre.y + dialRadius * std::sin(toAngle - pi / 2.0f));
  juce::Path dialPath;
  dialPath.startNewSubPath(dialStart);
  dialPath.lineTo(dialEnd);

  g.setColour(Colors::Knob::dial);
  g.strokePath(dialPath, strokeType);

  // Coloring outside arc to show what the value is based on
  // arc turned away from start angle
  if (slider.isEnabled()) {
    float fromAngle = rotaryStartAngle;
    if (slider.getProperties()["drawFromMiddle"])
      fromAngle += (rotaryEndAngle - rotaryStartAngle) / 2.0f;

    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                           fromAngle, toAngle, true);

    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    g.strokePath(valueArc, strokeType);
  }
}

juce::Font
RotaryKnobLookAndFeel::getLabelFont([[maybe_unused]] juce::Label &label) {
  return Fonts::getFont();
}

// A class created here means that only fucntions inside this .cpp file
// will use it and not others. Classes defined in .h file is used in
// other places.
class RotaryKnobLabel : public juce::Label {
public:
  RotaryKnobLabel() : juce::Label() {}

  void mouseWheelMove(const juce::MouseEvent &,
                      const juce::MouseWheelDetails &) override {}

  std::unique_ptr<juce::AccessibilityHandler>
  createAccessibilityHandler() override {
    return createIgnoredAccessibilityHandler(*this);
  }

  juce::TextEditor *createEditorComponent() override {
    auto *editor = new juce::TextEditor(getName());
    editor->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
    copyAllExplicitColoursTo(*editor);

    editor->setBorder(juce::BorderSize<int>());
    editor->setIndents(2, 1);
    editor->setJustification(juce::Justification::centredTop);
    editor->setPopupMenuEnabled(false);
    editor->setInputRestrictions(8);
    return editor;
  }
};

juce::Label *RotaryKnobLookAndFeel::createSliderTextBox(juce::Slider &slider) {
  auto l = new RotaryKnobLabel();
  l->setJustificationType(juce::Justification::centred);
  l->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
  l->setColour(juce::Label::textColourId,
               slider.findColour(juce::Slider::textBoxTextColourId));
  l->setColour(juce::TextEditor::textColourId, Colors::Knob::value);
  l->setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
  l->setColour(juce::TextEditor::highlightColourId,
               slider.findColour(juce::Slider::rotarySliderFillColourId));
  l->setColour(juce::TextEditor::backgroundColourId,
               Colors::Knob::textBoxBackground);
  return l;
}

void RotaryKnobLookAndFeel::fillTextEditorBackground(juce::Graphics &g,
                                                     int width, int height,
                                                     juce::TextEditor &editor) {
  g.setColour(Colors::Knob::textBoxBackground);
  g.fillRoundedRectangle(editor.getLocalBounds().reduced(4, 0).toFloat(), 4.0f);
}

MainLookAndFeel::MainLookAndFeel() {
  setColour(juce::GroupComponent::textColourId, Colors::Group::label);
  setColour(juce::GroupComponent::outlineColourId, Colors::Group::outline);
}

juce::Font MainLookAndFeel::getLabelFont([[maybe_unused]] juce::Label &label) {
  return Fonts::getFont();
}
