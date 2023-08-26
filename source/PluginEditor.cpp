#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), _pluginProcessor (p)
{
    juce::ignoreUnused (_pluginProcessor);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // Set up Tape Stop slider
    addAndMakeVisible (gainSlider);
    gainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    gainSlider.setRange (0.0, 1.0); // Adjust the range as needed
    gainSlider.setValue (0.0);
    gainSlider.onValueChange = [this]
    {
        _pluginProcessor.setGain ((float) gainSlider.getValue());
    };
}

PluginEditor::~PluginEditor()
{
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Gain", getLocalBounds(), juce::Justification::centred, 1);
}

void PluginEditor::resized()
{
    // lay out the positions of your components
    gainSlider.setBounds (10, 10, getWidth() - 20, 20);
}
