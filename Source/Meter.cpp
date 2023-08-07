#include "Meter.h"

void Meter::paint (juce::Graphics& graphics)
{
    auto bounds = getLocalBounds().toFloat();
    graphics.setColour (juce::Colours::white.withAlpha (0.5f));
    graphics.fillRoundedRectangle (bounds, 5.0f);
}