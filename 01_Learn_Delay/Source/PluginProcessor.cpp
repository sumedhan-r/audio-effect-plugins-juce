/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

enum class gainImplementationChoice { PRE_BUILT, CUSTOM };

_01_Learn_DelayAudioProcessor::_01_Learn_DelayAudioProcessor()
    : juce::AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      params(apvts) {}

_01_Learn_DelayAudioProcessor::~_01_Learn_DelayAudioProcessor() {}

//==============================================================================
const juce::String _01_Learn_DelayAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool _01_Learn_DelayAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool _01_Learn_DelayAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool _01_Learn_DelayAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double _01_Learn_DelayAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int _01_Learn_DelayAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int _01_Learn_DelayAudioProcessor::getCurrentProgram() { return 0; }

void _01_Learn_DelayAudioProcessor::setCurrentProgram(int index) {}

const juce::String _01_Learn_DelayAudioProcessor::getProgramName(int index) {
  return {};
}

void _01_Learn_DelayAudioProcessor::changeProgramName(
    int index, const juce::String &newName) {}

//==============================================================================
void _01_Learn_DelayAudioProcessor::prepareToPlay(double sampleRate,
                                                  int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  params.prepareToPlay(sampleRate);
  params.reset();
}

void _01_Learn_DelayAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool _01_Learn_DelayAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void _01_Learn_DelayAudioProcessor::processBlock(
    juce::AudioBuffer<float> &buffer,
    [[maybe_unused]] juce::MidiBuffer &midiMessages) {
  // DeNormals are values below lowerst floating point. This ensures that
  // float is approximated to zero at values < 10^(-38) and does not lead
  // to sudden CPU usage spike.
  juce::ScopedNoDenormals noDenormals;

  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  // Make sure to reset the state if your inner loop is processing
  // the samples and the outer loop is handling the channels.
  // Alternatively, you can process the samples with the channels
  // interleaved by keeping the same state.

  /* Two implementations of Gain
      1) Using pre-built function
      2) Using hand-written code

   Default : Hand-written code is invoked
  */

  params.update();

  gainImplementationChoice choice = gainImplementationChoice::CUSTOM;

  if (choice == gainImplementationChoice::PRE_BUILT) {
    // Gain method 1 : Pre-built function
    buffer.applyGain(params.gain);
  } else if (choice == gainImplementationChoice::CUSTOM) {
    // Gain method 2 : Custom function
    float *channelDataL = buffer.getWritePointer(0);
    float *channelDataR = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
      // Multiple the gain for each sample recursively in the
      // entire channel buffer/block
      params.smoothen();

      channelDataL[sample] *= params.gain;
      channelDataR[sample] *= params.gain;
    }
  }
}

//==============================================================================
bool _01_Learn_DelayAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *_01_Learn_DelayAudioProcessor::createEditor() {
  return new _01_Learn_DelayAudioProcessorEditor(*this);
}

//==============================================================================
void _01_Learn_DelayAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void _01_Learn_DelayAudioProcessor::setStateInformation(const void *data,
                                                        int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

  if (xml != nullptr && xml->hasTagName(apvts.state.getType())) {
    apvts.replaceState(juce::ValueTree::fromXml(*xml));
  }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new _01_Learn_DelayAudioProcessor();
}
