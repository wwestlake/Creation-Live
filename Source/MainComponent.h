#pragma once

#include <JuceHeader.h>
#include <creation/ui/CreationSuiteHeaderBar.h>

class MainComponent final : public juce::Component
{
public:
    MainComponent();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void syncTransportState();

    CreationSuiteHeaderBar headerBar;
    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label runtimeLabel;
    juce::GroupComponent timelineGroup;
    juce::GroupComponent previewGroup;
    juce::GroupComponent libraryGroup;
    juce::TextEditor notesBox;
    bool playing = false;
    bool recording = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

