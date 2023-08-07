#pragma once

class Meter : public juce::Component
{
public:
    Meter() = default;
    ~Meter() = default;

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        g.setColour (juce::Colours::white.withBrightness (0.4f));
        g.fillRoundedRectangle (bounds, 5.0f);
    }
};
