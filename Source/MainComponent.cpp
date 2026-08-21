#include "MainComponent.h"
#include "Branding.h"
#include "../Language/AppLanguagePolicy.h"
#include <creation/ui/CreationSuiteLogos.h>

namespace
{
// Wraps an existing component (not owned) as a dock panel's content, filling
// whatever bounds the dock zone/tab gives it.
class NonOwningPanelHost final : public juce::Component
{
public:
    explicit NonOwningPanelHost(juce::Component& contentToHost) : content(contentToHost)
    {
        addAndMakeVisible(content);
    }

    void resized() override
    {
        content.setBounds(getLocalBounds());
    }

private:
    juce::Component& content;
};

const juce::String panelIdTimeline = "timeline";
const juce::String panelIdPreview = "preview";
const juce::String panelIdLibrary = "library";
const juce::String panelIdNotes = "notes";

constexpr int menuIdPanelTimeline = 3001;
constexpr int menuIdPanelPreview = 3002;
constexpr int menuIdPanelLibrary = 3003;
constexpr int menuIdPanelNotes = 3004;
constexpr int menuIdResetLayout = 3005;
}

MainComponent::MainComponent()
{
    juce::String suiteError;
    suiteSettings = suiteSettingsStore.load(suiteError);

    headerBar.setAppTitle("Creation Live");
    headerBar.setLogoImage(creation::ui::getSuiteLogoImage(creation::ui::SuiteLogoId::live));
    headerBar.setProjectLabel("Project: Untitled Live Show");
    headerBar.audioButton.setButtonText("Live");
    headerBar.tourButton.setButtonText("Tools");
    headerBar.setTransportButtonVisible(CreationSuiteHeaderBar::TransportButtonSlot::rewind, false);
    headerBar.setTransportButtonVisible(CreationSuiteHeaderBar::TransportButtonSlot::fastForward, false);
    headerBar.setTransportButtonEnabled(CreationSuiteHeaderBar::TransportButtonSlot::click, false);
    headerBar.onPlay = [this]
    {
        playing = true;
        recording = false;
        syncTransportState();
        headerBar.setStatusText("Transport: go live preview");
    };
    headerBar.onPause = [this]
    {
        playing = false;
        recording = false;
        syncTransportState();
        headerBar.setStatusText("Transport: pause");
    };
    headerBar.onStop = [this]
    {
        playing = false;
        recording = false;
        syncTransportState();
        headerBar.setStatusText("Transport: stop");
    };
    headerBar.onRecord = [this]
    {
        recording = ! recording;
        playing = recording;
        syncTransportState();
        headerBar.setStatusText(recording ? "Transport: recording broadcast" : "Transport: record off");
    };
    headerBar.onLoopChanged = [this](bool enabled)
    {
        headerBar.setStatusText(enabled ? "Loop mode enabled" : "Loop mode disabled");
    };
    headerBar.setStatusText("Idle");

    suiteShellController.attach(headerBar,
                                {
                                    "Creation Live",
                                    creation::assets::SuiteAppDomain::live,
                                    creation_live::branding::backgroundColour()
                                },
                                [this](const juce::String& status)
                                {
                                    headerBar.setStatusText(status);
                                });

    suiteShellController.onProjectOpenRequested = [this](const juce::String& projectId)
    {
        openProject(projectId);
    };

    addAndMakeVisible(headerBar);
    syncTransportState();

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
    // Reparented into dock panels below (see initialiseDockingWorkspace), not
    // added directly here.

    notesBox.setMultiLine(true);
    notesBox.setReadOnly(true);
    notesBox.setText("Shared language domain: live\nAllowed node domains: shared, live, scene, broadcast\n\n"
                     "Next steps:\n"
                     "- scene graph + routing\n"
                     "- cues / automation\n"
                     "- media + overlay playback\n"
                     "- stream health + transitions\n");

    menuBar = std::make_unique<juce::MenuBarComponent>(static_cast<juce::MenuBarModel*>(this));
    // Nothing in this app sets a suite-wide dark LookAndFeel, so MenuBarComponent
    // falls back to LookAndFeel_V4::drawMenuBarItem/drawMenuBarBackground, which key
    // off TextButton colour ids (not PopupMenu's) -- the default scheme renders dark
    // text on a dark bar, invisible against this app's dark theme without this.
    menuBar->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1c2230));
    menuBar->setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff2a3244));
    menuBar->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    menuBar->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addAndMakeVisible(*menuBar);

    dockManager = std::make_unique<CreationDock::DockManager>(*this);
    addAndMakeVisible(*dockManager);
    initialiseDockingWorkspace();
    // setSize() below fires resized() immediately; menuBar/dockManager must already
    // exist and be registered before that happens, or they're silently left at zero
    // bounds (addAndMakeVisible alone doesn't trigger a layout pass).
    resized();

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
    headerBar.setBounds(getLocalBounds().removeFromTop(96));

    auto area = getLocalBounds();
    area.removeFromTop(96);

    auto titleArea = area.removeFromTop(60).reduced(36, 4);
    titleLabel.setBounds(titleArea.removeFromTop(28));
    subtitleLabel.setBounds(titleArea.removeFromTop(18));
    runtimeLabel.setBounds(titleArea);

    if (menuBar != nullptr)
        menuBar->setBounds(area.removeFromTop(28));

    if (dockManager != nullptr)
        dockManager->setBounds(area);
}

juce::StringArray MainComponent::getMenuBarNames()
{
    return { "Panels" };
}

juce::PopupMenu MainComponent::getMenuForIndex(int, const juce::String&)
{
    juce::PopupMenu menu;

    const auto isOpen = [this](const juce::String& id)
    {
        return dockManager != nullptr && dockManager->isPanelOpen(id);
    };

    menu.addItem(menuIdPanelTimeline, "Run Of Show / Cue Stack", true, isOpen(panelIdTimeline));
    menu.addItem(menuIdPanelPreview, "Program / Multiview", true, isOpen(panelIdPreview));
    menu.addItem(menuIdPanelLibrary, "Scenes / Media / Overlays", true, isOpen(panelIdLibrary));
    menu.addItem(menuIdPanelNotes, "Notes", true, isOpen(panelIdNotes));
    menu.addSeparator();
    menu.addItem(menuIdResetLayout, "Reset Dock Layout");
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int)
{
    switch (menuItemID)
    {
        case menuIdPanelTimeline: toggleDockPanel(panelIdTimeline, CreationDock::DockTargetZone::Bottom); break;
        case menuIdPanelPreview:  toggleDockPanel(panelIdPreview, CreationDock::DockTargetZone::CenterTab); break;
        case menuIdPanelLibrary:  toggleDockPanel(panelIdLibrary, CreationDock::DockTargetZone::Right); break;
        case menuIdPanelNotes:    toggleDockPanel(panelIdNotes, CreationDock::DockTargetZone::Right); break;
        case menuIdResetLayout:   if (dockManager != nullptr) dockManager->resetLayout(); break;
        default: break;
    }

    menuItemsChanged();
}

void MainComponent::initialiseDockingWorkspace()
{
    if (dockManager == nullptr)
        return;

    // These three GroupComponents are empty scaffold frames today -- docking
    // gives them a real resizable/closable window, but there's no real content
    // inside them yet; that's future work, not part of this pass.
    dockManager->registerPanel(panelIdPreview, "Program / Multiview",
        std::make_unique<NonOwningPanelHost>(previewGroup), CreationDock::DockTargetZone::CenterTab);
    dockManager->registerPanel(panelIdLibrary, "Scenes / Media / Overlays",
        std::make_unique<NonOwningPanelHost>(libraryGroup), CreationDock::DockTargetZone::Right);
    dockManager->registerPanel(panelIdTimeline, "Run Of Show / Cue Stack",
        std::make_unique<NonOwningPanelHost>(timelineGroup), CreationDock::DockTargetZone::Bottom);
    dockManager->registerPanel(panelIdNotes, "Notes",
        std::make_unique<NonOwningPanelHost>(notesBox), CreationDock::DockTargetZone::Right);
}

void MainComponent::toggleDockPanel(const juce::String& panelId, CreationDock::DockTargetZone fallbackZone)
{
    if (dockManager == nullptr)
        return;

    if (dockManager->isPanelOpen(panelId))
        dockManager->closePanel(panelId);
    else
        dockManager->showPanel(panelId, fallbackZone);

    menuItemsChanged();
}

void MainComponent::openProject(const juce::String& projectId)
{
    juce::String errorMessage;
    if (! creation::assets::ProjectWorkspaceService::openProject(suiteSettings, projectId, projectSession, errorMessage))
    {
        headerBar.setStatusText("Could not open project: " + errorMessage);
        return;
    }

    headerBar.setProjectLabel("Project: " + projectSession.getManifest().projectName);
    headerBar.setStatusText("Opened project: " + projectSession.getManifest().projectName);
}

void MainComponent::syncTransportState()
{
    headerBar.setPlaybackVisualState(playing, recording);
    headerBar.setScrubModeEnabled(false);
}
