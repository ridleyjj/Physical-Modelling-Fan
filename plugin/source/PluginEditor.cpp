#include "PhysicalModellingFan/PluginProcessor.h"
#include "PhysicalModellingFan/PluginEditor.h"
#include <memory>
#include <PhysicalModellingFan/utils/jr_juceUtils.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), presetPanel(p.getPresetManager()), fanControls(p), sharedControls(p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&myLookAndFeel);
    setLookAndFeel(&myLookAndFeel);

    fanControls.init();
    sharedControls.init();

    addAndMakeVisible(presetPanel);
    addAndMakeVisible(fanControls);
    addAndMakeVisible(sharedControls);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{

    // top presets row
    presetPanel.setBoundsRelative(0.0f, 0.0f, 1.0f, 0.1f);

    fanControls.setBoundsRelative(0.0f, 0.1f, 0.5f, 0.9f);
    sharedControls.setBoundsRelative(0.5f, 0.1f, 0.5f, 0.9f);
}
