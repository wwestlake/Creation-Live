#include "MainComponent.h"
#include "Branding.h"
#include "../Language/AppLanguagePolicy.h"

MainComponent::MainComponent()
{
    titleLabel.setText("Creation Live", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(32.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("Live streaming, scene control, media playback, and broadcast automation on the shared Creation platform.",
                          juce::dontSendNotification);
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc5cfde));
    addAndMakeVisible(subtitleLabel);

    runtimeLabel.setText(creation_live::language::getLanguageRuntimeSummary(), juce::dontSendNotification);
    runtimeLabel.setColour(juce::Label::textColourId, creation_live::branding::accentColour());
    addAndMakeVisible(runtimeLabel);

    timelineGroup.setText("Run Of Show / Cue Stack");
    previewGroup.setText("Program / Multiview");
    libraryGroup.setText("Scenes / Media / Overlays");
    addAndMakeVisible(timelineGroup);
    addAndMakeVisible(previewGroup);
    addAndMakeVisible(libraryGroup);

    notesBox.setMultiLine(true);
    notesBox.setReadOnly(true);
    notesBox.setText("Shared language domain: live\nAllowed node domains: shared, live, scene, broadcast\n\n"
                     "Next steps:\n"
                     "- scene graph + routing\n"
                     "- cues / automation\n"
                     "- media + overlay playback\n"
                     "- stream health + transitions\n");
    addAndMakeVisible(notesBox);

    setSize(1280, 820);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(creation_live::branding::backgroundColour());

    auto bounds = getLocalBounds().toFloat().reduced(18.0f);
    g.setColour(creation_live::branding::panelColour());
    g.fillRoundedRectangle(bounds, 24.0f);

    g.setColour(creation_live::branding::accentColour().withAlpha(0.85f));
    g.drawRoundedRectangle(bounds, 24.0f, 1.5f);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(36, 28);
    titleLabel.setBounds(area.removeFromTop(40));
    subtitleLabel.setBounds(area.removeFromTop(28));
    runtimeLabel.setBounds(area.removeFromTop(26));
    area.removeFromTop(18);

    auto topRow = area.removeFromTop(320);
    previewGroup.setBounds(topRow.removeFromLeft(area.getWidth() * 2 / 3).reduced(0, 0));
    topRow.removeFromLeft(14);
    libraryGroup.setBounds(topRow);

    area.removeFromTop(14);
    timelineGroup.setBounds(area.removeFromTop(240));
    area.removeFromTop(14);
    notesBox.setBounds(area);
}
