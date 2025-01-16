#pragma once

#include "PluginProcessor.h"
#include <memory>
#include <PhysicalModellingFan/components/gui/MirrorSliderAttachment.h>
#include <PhysicalModellingFan/components/gui/PresetPanel.h>
#include <PhysicalModellingFan/components/gui/jr_FanControls.h>
#include <PhysicalModellingFan/components/gui/jr_SharedControls.h>

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor &processorRef;

    jr::FanControls fanControls;
    jr::SharedControls sharedControls;
    jr::PresetPanel presetPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
