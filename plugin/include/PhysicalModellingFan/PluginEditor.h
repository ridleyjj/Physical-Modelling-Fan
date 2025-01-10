#pragma once

#include "PluginProcessor.h"
#include <memory>
#include <PhysicalModellingFan/components/gui/MirrorSliderAttachment.h>

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void initSimpleSlider(juce::Slider *slider, juce::Label *label, const juce::String &name);

    void initSimpleSliderWithRange(juce::Slider *slider, juce::Label *label, const juce::String &name, double minVal, double maxVal, double step);

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
    juce::Slider gainSlider{juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow};
    juce::Slider fanToneSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider fanNoiseSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
    juce::Slider fanWidthSlider{juce::Slider::SliderStyle::TwoValueHorizontal, juce::Slider::NoTextBox};

    juce::Label gainLabel, speedLabel, fanToneLabel, fanNoiseLabel, fanWidthLabel, powerDownTimeLabel, powerUpTimeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment, speedAttachment, fanToneAttachment, fanNoiseAttachment, powerDownTimeAttachment, powerUpTimeAttachment;
    std::unique_ptr<jr::MirrorSliderAttachment> fanWidthAttachment;

    // Buttons & Labels

    juce::ToggleButton fanDopplerButton{"Doppler On/Off"};
    juce::ToggleButton powerButton{"Power On/Off"};

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> dopplerAttachment, powerButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
