/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

enum class gainImplementationChoice { PRE_BUILT, CUSTOM };

//==============================================================================
A0LearnDelayAudioProcessor::A0LearnDelayAudioProcessor()
    : juce::AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      params(apvts) {}

A0LearnDelayAudioProcessor::~A0LearnDelayAudioProcessor() {}

//==============================================================================
const juce::String A0LearnDelayAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool A0LearnDelayAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool A0LearnDelayAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool A0LearnDelayAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double A0LearnDelayAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int A0LearnDelayAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int A0LearnDelayAudioProcessor::getCurrentProgram() { return 0; }

void A0LearnDelayAudioProcessor::setCurrentProgram(int index) {}

const juce::String A0LearnDelayAudioProcessor::getProgramName(int index) {
  return {};
}

void A0LearnDelayAudioProcessor::changeProgramName(
    int index, const juce::String &newName) {}

//==============================================================================
void A0LearnDelayAudioProcessor::prepareToPlay(double sampleRate,
                                               int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  params.prepareToPlay(sampleRate);
  params.reset();

  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = juce::uint32(samplesPerBlock);
  spec.numChannels = 2;

  delayLine.prepare(spec);

  double numSamples = (Parameters::maxDelayTime / 1000.0) * sampleRate;
  int maxDelayInSamples = int(std::ceil(numSamples));
  delayLine.setMaximumDelayInSamples(maxDelayInSamples);
  delayLine.reset();
}

void A0LearnDelayAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A0LearnDelayAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void A0LearnDelayAudioProcessor::processBlock(
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

  float sampleRate = float(getSampleRate());

  gainImplementationChoice choice = gainImplementationChoice::CUSTOM;

  if (choice == gainImplementationChoice::PRE_BUILT) {
    // Gain method 1 : Pre-built function
    buffer.applyGain(params.gain);
  } else if (choice == gainImplementationChoice::CUSTOM) {
    // Gain method 2 : Custom function
    float *channelDataL = buffer.getWritePointer(0);
    float *channelDataR = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
      /*
       Plug-in : Gain

       Multiply the gain for each sample recursively in the
       entire channel buffer/block
      */
      params.smoothen();

      /*
       Plug-in : Delay

       One-pole filter used for smoothing the delay time slider

       Dry and Wet samples are interfered for ...
      */
      float delayInSamples = (params.delayTime / 1000.0f) * sampleRate;
      delayLine.setDelay(delayInSamples);

      float dryL = channelDataL[sample];
      float dryR = channelDataR[sample];

      delayLine.pushSample(0, dryL);
      delayLine.pushSample(1, dryR);

      float wetL = delayLine.popSample(0);
      float wetR = delayLine.popSample(1);

      float mixL = dryL + wetL * params.mix;
      float mixR = dryR + wetR * params.mix;

      channelDataL[sample] = mixL * params.gain;
      channelDataR[sample] = mixR * params.gain;
    }
  }
}

//==============================================================================
bool A0LearnDelayAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *A0LearnDelayAudioProcessor::createEditor() {
  return new A0LearnDelayAudioProcessorEditor(*this);
}

//==============================================================================
void A0LearnDelayAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void A0LearnDelayAudioProcessor::setStateInformation(const void *data,
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
  return new A0LearnDelayAudioProcessor();
}
