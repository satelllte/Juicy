#include "Meter.h"

void Meter::paint (juce::Graphics& graphics)
{
    const float alpha = 0.5F;
    const float borderRadius = 5.0F;

    auto bounds = getLocalBounds().toFloat();
    graphics.setColour (juce::Colours::white.withAlpha (alpha));
    graphics.fillRoundedRectangle (bounds, borderRadius);
}