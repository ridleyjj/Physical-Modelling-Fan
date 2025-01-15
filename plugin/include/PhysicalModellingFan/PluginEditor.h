#pragma once

#include "PluginProcessor.h"
#include <memory>
#include <PhysicalModellingFan/components/gui/MirrorSliderAttachment.h>
#include <PhysicalModellingFan/components/gui/PresetPanel.h>
#include <PhysicalModellingFan/components/gui/jr_FanControls.h>

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> createSliderAttachment(juce::String id, juce::Slider &slider);
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> createButtonAttachment(juce::String id, juce::Button &button);

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor &processorRef;

    // Sliders & Labels

    // shared
    juce::Slider speedSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider powerUpTimeSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider powerDownTimeSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};

    // fan
    // TODO - remove this fan gain slider and replace it with a master gain slider. Fan overall volume can simply be managed through tone and noise levels
    juce::Slider gainSlider{juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow};
    jr::FanControls fanControls;

    juce::Slider fanToneSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider fanNoiseSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider fanWidthSlider{juce::Slider::SliderStyle::TwoValueHorizontal, juce::Slider::NoTextBox};

    juce::Label gainLabel, speedLabel, fanToneLabel, fanNoiseLabel, fanWidthLabel, powerDownTimeLabel, powerUpTimeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment, speedAttachment, fanToneAttachment, fanNoiseAttachment, powerDownTimeAttachment, powerUpTimeAttachment;
    std::unique_ptr<jr::MirrorSliderAttachment> fanWidthAttachment;

    // Buttons & Labels

    juce::ToggleButton fanDopplerButton{"Doppler On/Off"};
    juce::ToggleButton powerButton{"Power On/Off"};

    // Presets

    jr::PresetPanel presetPanel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> dopplerAttachment, powerButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
