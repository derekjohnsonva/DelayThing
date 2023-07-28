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
                     std::make_unique<juce::AudioParameterInt>(delayTimeParamName, "Delay Time", 10, 2000, 200),
                 })
{
    delayTime = parameters.getRawParameterValue(delayTimeParamName);
    jassert(delayTime != nullptr);
    parameters.addParameterListener("delayTime", this);
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

int DelayThingAudioProcessor::setDelayBufferSizeInSamples(int newDelayBufferSizeInSamples)
{
    // set the buffer and return the old buffer size
    int oldDelayBufferSizeInSamples = delayBufferSizeInSamples;
    delayBufferSizeInSamples = newDelayBufferSizeInSamples;
    return oldDelayBufferSizeInSamples;
}
int DelayThingAudioProcessor::getDelayBufferSizeInSamples() const
{
    return delayBufferSizeInSamples;
}

void DelayThingAudioProcessor::setDelayBufferSize(int numChannels, int numSamples)
{
    delayBuffer.setSize(numChannels, numSamples);
}

void DelayThingAudioProcessor::setDelayBufferSample(int channel, int sample, float value)
{
    delayBuffer.setSample(channel, sample, value);
}

juce::AudioBuffer<float> DelayThingAudioProcessor::getDelayBuffer() const
{
    return delayBuffer;
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
    setDelayBufferSizeInSamples(newDelayBufferSizeInSamples);
}

//==============================================================================
void DelayThingAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // we will need our buffer to be able to hold 2 seconds of audio data
    // First, get the number of samples in 2 seconds of audio (+ 2 blocks for safety)
    int numSamples = (int)(2.0 * sampleRate) + (2 * samplesPerBlock);
    delayBuffer.setSize(getTotalNumOutputChannels(), numSamples);
    delayBuffer.clear();

    // set the write head positions to zero for every input channel
    delayBufferWritePositions.resize(getTotalNumInputChannels());
    for (int i = 0; i < getTotalNumInputChannels(); ++i)
    {
        delayBufferWritePositions[i] = 0;
    }

    delayTime = parameters.getRawParameterValue(delayTimeParamName);
    updateDelayBufferSizeInSamples(*delayTime);
}

void DelayThingAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == delayTimeParamName)
    {
        // cast the new value to be an int
        // convert newValue to an atomic<int> *

        *delayTime = newValue;
        updateDelayBufferSizeInSamples(*delayTime);
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
        jassert(buffer.getNumSamples() <= delayBufferSizeInSamples);

        // add the channel data to the delay buffer
        writeToDelayBuffer(buffer, channel, delayBufferWritePositions[channel]);
        jassert(delayBufferWritePositions[channel] >= 0 && delayBufferWritePositions[channel] < delayBuffer.getNumSamples());

        // calculate the read position in the delay
        // the read position is the oldest sample in the delay buffer.
        int readPosition = delayBufferWritePositions[channel] - delayBufferSizeInSamples;
        // // if the read position is negative, wrap it around to the end of the buffer
        if (readPosition < 0)
        {
            readPosition += delayBuffer.getNumSamples();
        }
        //        jassert(readPosition >= 0 && readPosition < delayBufferSizeInSamples);
        // read from the delay buffer
        addFromDelayBuffer(buffer, channel, readPosition);

        // Update the write positions
        delayBufferWritePositions[channel] += buffer.getNumSamples();
        delayBufferWritePositions[channel] %= delayBuffer.getNumSamples();
    }
}

void DelayThingAudioProcessor::writeToDelayBuffer(const juce::AudioBuffer<float> &inputBuffer, int channel, int writePosition)
{
    // check to see if when we write to the delay buffer we will be writing past the end
    // of the buffer
    // if we are, we need to wrap around to the beginning of the buffer
    if (writePosition + inputBuffer.getNumSamples() >= delayBuffer.getNumSamples())
    {
        // calculate the number of samples we will write to the end of the buffer
        int numSamplesToEnd = delayBuffer.getNumSamples() - writePosition;
        // write the samples to the end of the buffer
        jassert(numSamplesToEnd >= 0);
        delayBuffer.copyFrom(channel, writePosition, inputBuffer, channel, 0, numSamplesToEnd);
        // calculate the number of samples we will write to the beginning of the buffer
        int numSamplesFromStart = inputBuffer.getNumSamples() - numSamplesToEnd;
        // write the samples to the beginning of the buffer
        jassert(numSamplesFromStart >= 0);
        delayBuffer.copyFrom(channel, 0, inputBuffer, channel, numSamplesToEnd, numSamplesFromStart);
    }
    else
    {
        // if we are not writing past the end of the buffer, just write the samples
        // to the buffer
        delayBuffer.copyFrom(channel, writePosition, inputBuffer, channel, 0, inputBuffer.getNumSamples());
    }
}

void DelayThingAudioProcessor::addFromDelayBuffer(juce::AudioBuffer<float> &outputBuffer, int channel, int readPosition)
{
    // Throw an error if the size of the delayBufferSizeInSamples is smaller that the outputBuffer
    jassert(delayBufferSizeInSamples >= outputBuffer.getNumSamples());
    // check to see if when we read from the delay buffer we will be reading past the end
    // of the buffer
    // if we are, we need to wrap around to the beginning of the buffer
    if (readPosition + outputBuffer.getNumSamples() >= delayBuffer.getNumSamples())
    {
        // calculate the number of samples we will read from the end of the buffer
        int numSamplesToEnd = delayBuffer.getNumSamples() - readPosition;
        // read the samples from the end of the buffer
        outputBuffer.addFrom(channel, 0, delayBuffer, channel, readPosition, numSamplesToEnd);
        // calculate the number of samples we will read from the beginning of the buffer
        int numSamplesFromStart = outputBuffer.getNumSamples() - numSamplesToEnd;
        // read the samples from the beginning of the buffer
        outputBuffer.addFrom(channel, numSamplesToEnd, delayBuffer, channel, 0, numSamplesFromStart);
    }
    else
    {
        // if we are not reading past the end of the buffer, just read the samples
        // from the buffer
        // outputBuffer.addSample(channel, 0, delayBuffer.getSample(channel, readPosition));
        outputBuffer.addFrom(channel, 0, delayBuffer, channel, readPosition, outputBuffer.getNumSamples());
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
