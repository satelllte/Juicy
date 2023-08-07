#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible (meter);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    const int width = 400;
    const int height = 300;
    setSize (width, height);
}

PluginEditor::~PluginEditor()
{
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Juicy", getLocalBounds(), juce::Justification::centred, 1);
}

void PluginEditor::resized()
{
    meter.setBounds (200, 150, 100, 20);
}
