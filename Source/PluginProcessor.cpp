#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayThingAudioProcessor::DelayThingAudioProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      parameters(*this, &undoManager, juce::Identifier("DelayThingParameters"),
                 {
                     std::make_unique<juce::AudioParameterInt>(delayTimeParamName, "Time", 10, 2000, 200),
                     std::make_unique<juce::AudioParameterFloat>(delayMixParamName, "Mix", 0.0f, 2.0f, 1.0f),
                     std::make_unique<juce::AudioParameterInt>(delayRepsParamName, "Repetitions", 1, 5, 2),
                     std::make_unique<juce::AudioParameterFloat>(delayRepGain1ParamName, "Rep 1 Gain", 0.0f, 2.0f, 0.5f),
                     std::make_unique<juce::AudioParameterFloat>(delayRepGain2ParamName, "Rep 2 Gain", 0.0f, 2.0f, 0.5f),
                     std::make_unique<juce::AudioParameterFloat>(delayRepGain3ParamName, "Rep 3 Gain", 0.0f, 2.0f, 0.5f),
                     std::make_unique<juce::AudioParameterFloat>(delayRepGain4ParamName, "Rep 4 Gain", 0.0f, 2.0f, 0.5f),
                     std::make_unique<juce::AudioParameterFloat>(delayRepGain5ParamName, "Rep 5 Gain", 0.0f, 2.0f, 0.5f),
                 }),
      delayBuffers(2),
      repGains()
{

    delayTime = parameters.getRawParameterValue(delayTimeParamName);
    jassert(delayTime != nullptr);
    parameters.addParameterListener(delayTimeParamName, this);

    delayMix = parameters.getRawParameterValue(delayMixParamName);
    jassert(delayMix != nullptr);
    parameters.addParameterListener(delayMixParamName, this);

    delayReps = parameters.getRawParameterValue(delayRepsParamName);
    jassert(delayReps != nullptr);
    parameters.addParameterListener(delayRepsParamName, this);
    // TODO: There is probably a cleaner way to do this
    repGains.set(0, parameters.getRawParameterValue(delayRepGain1ParamName));
    parameters.addParameterListener(delayRepGain1ParamName, this);
    repGains.set(1, parameters.getRawParameterValue(delayRepGain2ParamName));
    parameters.addParameterListener(delayRepGain1ParamName, this);
    repGains.set(2, parameters.getRawParameterValue(delayRepGain3ParamName));
    parameters.addParameterListener(delayRepGain1ParamName, this);
    repGains.set(3, parameters.getRawParameterValue(delayRepGain4ParamName));
    parameters.addParameterListener(delayRepGain1ParamName, this);
    repGains.set(4, parameters.getRawParameterValue(delayRepGain5ParamName));
    parameters.addParameterListener(delayRepGain1ParamName, this);
}

DelayThingAudioProcessor::~DelayThingAudioProcessor() = default;

//==============================================================================
const juce::String DelayThingAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayThingAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DelayThingAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DelayThingAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DelayThingAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayThingAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int DelayThingAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayThingAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String DelayThingAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void DelayThingAudioProcessor::setDelayBufferSize(int numChannels, int numSamples)
{
    // The number of channels is the number of delay buffers
    delayBuffers.resize(numChannels);
    // the number of samples is the size of each delay buffer
    for (auto &delayBuffer : delayBuffers)
    {
        delayBuffer.setSize(numSamples);
    }
}

void DelayThingAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

void DelayThingAudioProcessor::updateDelayBufferSizeInSamples(float delaySizeInMS)
{
    // calculate the new delay buffer size in samples
    int newDelayBufferSizeInSamples = juce::roundToInt(delaySizeInMS * getSampleRate() / 1000.0);
    // set the new delay buffer size in samples
    delayBufferSizeInSamples.setTarget(newDelayBufferSizeInSamples);
}

//==============================================================================
void DelayThingAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // we will need our buffer to be able to hold 2 seconds of audio data
    // First, get the number of samples in 2 seconds of audio (+ 2 blocks for safety)
    int numSamples = (int)(2.0 * sampleRate) + (2 * samplesPerBlock);

    setDelayBufferSize(getTotalNumInputChannels(), numSamples);

    delayTime = parameters.getRawParameterValue(delayTimeParamName);
    updateDelayBufferSizeInSamples(*delayTime);

    // initialize the repGains
    repGains.resize(maxDelayReps);
    repGains.set(0, parameters.getRawParameterValue(delayRepGain1ParamName));
    repGains.set(1, parameters.getRawParameterValue(delayRepGain2ParamName));
    repGains.set(2, parameters.getRawParameterValue(delayRepGain3ParamName));
    repGains.set(3, parameters.getRawParameterValue(delayRepGain4ParamName));
    repGains.set(4, parameters.getRawParameterValue(delayRepGain5ParamName));
}

void DelayThingAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == delayTimeParamName)
    {
        *delayTime = newValue;
        updateDelayBufferSizeInSamples(*delayTime);
    }
    else if (parameterID == delayMixParamName)
    {
        *delayMix = newValue;
    }
    else if (parameterID == delayRepsParamName)
    {
        *delayReps = newValue;
    }
    else if (parameterID == delayRepGain1ParamName)
    {
        *repGains[0] = newValue;
    }
    else if (parameterID == delayRepGain2ParamName)
    {
        *repGains[1] = newValue;
    }
    else if (parameterID == delayRepGain3ParamName)
    {
        *repGains[2] = newValue;
    }
    else if (parameterID == delayRepGain4ParamName)
    {
        *repGains[3] = newValue;
    }
    else if (parameterID == delayRepGain5ParamName)
    {
        *repGains[4] = newValue;
    }
}

juce::AudioProcessorValueTreeState &DelayThingAudioProcessor::getValueTreeState()
{
    return parameters;
}

void DelayThingAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool DelayThingAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void DelayThingAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);
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
    // This is the main audio processing loop
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // Throw an error if the size of the buffer is larger than the delayBufferSizeInSamples
        // add the channel data to the delay buffer
        delayBuffers[channel].writeFrom(buffer, channel);
        // read from the delay buffer
        delayBuffers[channel].addTo(buffer, channel, *delayReps, repGains, delayBufferSizeInSamples);
    }
}

//==============================================================================
bool DelayThingAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *DelayThingAudioProcessor::createEditor()
{
    return new DelayThingEditor(*this);
}

//==============================================================================
void DelayThingAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void DelayThingAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new DelayThingAudioProcessor();
}
