#pragma once

#include <JuceHeader.h>
#include <creation/assets/ProjectSession.h>
#include <creation/assets/ProjectWorkspaceService.h>
#include <creation/suite/SuiteSettings.h>
#include <creation/ui/CreationSuiteHeaderBar.h>
#include <creation/ui/SuiteShellController.h>
#include <CreationDock/DockManager.h>

class MainComponent final : public juce::Component,
                             private juce::MenuBarModel
{
public:
    MainComponent();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void syncTransportState();
    void openProject(const juce::String& projectId);

    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String&) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
    void initialiseDockingWorkspace();
    void toggleDockPanel(const juce::String& panelId, CreationDock::DockTargetZone fallbackZone);

    std::unique_ptr<juce::MenuBarComponent> menuBar;
    std::unique_ptr<CreationDock::DockManager> dockManager;

    CreationSuiteHeaderBar headerBar;
    creation::ui::SuiteShellController suiteShellController;
    creation::suite::SuiteSettingsStore suiteSettingsStore;
    creation::suite::SuiteSettings suiteSettings;
    creation::assets::ProjectSession projectSession;
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

