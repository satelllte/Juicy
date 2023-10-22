#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
#endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
{
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec processSpec;

    processSpec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    processSpec.sampleRate = sampleRate;
    processSpec.numChannels = 1;

    leftChain.prepare (processSpec);
    rightChain.prepare (processSpec);

    auto chainSettings = getChainSettings (apvts);

    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter (sampleRate, chainSettings.peakFrequency, chainSettings.peakQuality, juce::Decibels::decibelsToGain (chainSettings.peakGainInDecibels));
    *leftChain.get<ChainPositions::PeakFilter>().coefficients = *peakCoefficients;
    *rightChain.get<ChainPositions::PeakFilter>().coefficients = *peakCoefficients;
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

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
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

    /**
     * TODO: refactor this, so it doesn't repeat the same code from `prepareToPlay` method
     */
    // Peak filter
    auto chainSettings = getChainSettings (apvts);
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter (getSampleRate(), chainSettings.peakFrequency, chainSettings.peakQuality, juce::Decibels::decibelsToGain (chainSettings.peakGainInDecibels));
    *leftChain.get<ChainPositions::PeakFilter>().coefficients = *peakCoefficients;
    *rightChain.get<ChainPositions::PeakFilter>().coefficients = *peakCoefficients;

    // Apply audio processing
    auto block = juce::dsp::AudioBlock<float> (buffer);
    auto leftChannelBlock = block.getSingleChannelBlock (0);
    auto rightChannelBlock = block.getSingleChannelBlock (1);
    auto leftChannelContext = juce::dsp::ProcessContextReplacing<float> (leftChannelBlock);
    auto rightChannelContext = juce::dsp::ProcessContextReplacing<float> (rightChannelBlock);
    leftChain.process (leftChannelContext);
    rightChain.process (rightChannelContext);
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    /**
     * TODO: Once UI is implemented in `PluginEditor.h`, use it instead
     */
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    auto frequencyNormalisableRange = juce::NormalisableRange<float> (20.0f, 20000.0f, 0.1f);
    frequencyNormalisableRange.setSkewForCentre (1000.0f);

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> ("Low-Cut Frequency", "Low-Cut Frequency", frequencyNormalisableRange, 20.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> ("Low-Cut Slope", "Low-Cut Slope", juce::StringArray { "12 db/Oct", "24 db/Oct", "36 db/Oct", "48 db/Oct" }, 0));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("High-Cut Frequency", "High-Cut Frequency", frequencyNormalisableRange, 20000.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice> ("High-Cut Slope", "High-Cut Slope", juce::StringArray { "12 db/Oct", "24 db/Oct", "36 db/Oct", "48 db/Oct" }, 0));

    layout.add (std::make_unique<juce::AudioParameterFloat> ("Peak Frequency", "Peak Frequency", frequencyNormalisableRange, 750.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("Peak Gain", "Peak Gain", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.5f, 1.0f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("Peak Quality", "Peak Quality", juce::NormalisableRange<float> (0.1f, 10.0f, 0.05f, 1.0f), 1.0f));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

ChainSettings getChainSettings (const juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings chainSettings;

    chainSettings.lowCutFrequency = apvts.getRawParameterValue ("Low-Cut Frequency")->load();
    chainSettings.lowCutSlope = apvts.getRawParameterValue ("Low-Cut Slope")->load();
    chainSettings.highCutFrequency = apvts.getRawParameterValue ("High-Cut Frequency")->load();
    chainSettings.highCutSlope = apvts.getRawParameterValue ("High-Cut Slope")->load();
    chainSettings.peakFrequency = apvts.getRawParameterValue ("Peak Frequency")->load();
    chainSettings.peakGainInDecibels = apvts.getRawParameterValue ("Peak Gain")->load();
    chainSettings.peakQuality = apvts.getRawParameterValue ("Peak Quality")->load();

    return chainSettings;
}
