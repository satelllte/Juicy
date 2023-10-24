#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#if (MSVC)
#include "ipps.h"
#endif

enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48,
};

struct ChainSettings; // forward declaration
ChainSettings getChainSettings (const juce::AudioProcessorValueTreeState& apvts);

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
    using FilterCoefficients = Filter::CoefficientsPtr;

    MonoChain leftChain, rightChain;

    enum ChainPosition
    {
        LowCutFilter,
        PeakFilter,
        HighCutFilter
    };

    void updateFilters (const double sampleRate);
    void updatePeakFilter (const double sampleRate, const float peakFrequency, const float peakQuality, const float peakGainInDecibels);
    void updateLowCutFilter (const double sampleRate, const float frequency, const Slope slope);
    void updateHighCutFilter (const double sampleRate, const float frequency, const Slope slope);
    void updateCutFilter (
        const juce::ReferenceCountedArray<juce::dsp::FilterDesign<float>::IIRCoefficients>& coefficients,
        CutFilter* leftCutFilter,
        CutFilter* rightCutFilter,
        const Slope slope);
    void updateFilterCoefficients (FilterCoefficients& coefficientsToUpdate, const FilterCoefficients& coefficients);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

struct ChainSettings
{
    float lowCutFrequency { 0.0f };
    Slope lowCutSlope { Slope::Slope_12 };
    float highCutFrequency { 0.0f };
    Slope highCutSlope { Slope::Slope_12 };
    float peakFrequency { 0.0f };
    float peakGainInDecibels { 0.0f };
    float peakQuality { 0.0f };
};
