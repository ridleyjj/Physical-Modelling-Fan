#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <PhysicalModellingFan/components/gui/MirrorSliderAttachment.h>
#include <PhysicalModellingFan/utils/jr_juceUtils.h>

namespace jr
{
    class FanControls : public juce::Component
    {
    public:
        FanControls(AudioPluginAudioProcessor &p) : processorRef(p) {}

        void init()
        {
            jr::JuceUtils::initSimpleSlider(this, &toneSlider, &toneLabel, "Fan Tone");
            jr::JuceUtils::initSimpleSlider(this, &noiseSlider, &noiseLabel, "Fan Noise");
            jr::JuceUtils::initSimpleSliderWithRange(this, &widthSlider, &widthLabel, "Fan Stereo Width", -1.0, 1.0, 0.01);
            toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::FAN_TONE, toneSlider);
            noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::FAN_NOISE, noiseSlider);
            widthAttachment = std::make_unique<jr::MirrorSliderAttachment>(*(processorRef.getAPVTS().getParameter(ID::FAN_WIDTH)), widthSlider);
            addAndMakeVisible(dopplerButton);
            dopplerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ID::FAN_DOPPLER, dopplerButton);
        }

        void resized() override
        {
            const int marginY = 20;
            const int marginX = 4;
            const auto container = getLocalBounds().reduced(marginX, marginY);
            auto bounds = container;

            toneSlider.setBounds(bounds.removeFromTop(container.proportionOfHeight(0.34f)).reduced(marginX, marginY));
            noiseSlider.setBounds(bounds.removeFromTop(container.proportionOfHeight(0.34f)).reduced(marginX, marginY));
            auto bottomRow = bounds.removeFromTop(container.proportionOfHeight(0.33f)).reduced(marginX, marginY);
            widthSlider.setBounds(bottomRow.removeFromLeft(container.proportionOfWidth(0.75f)).reduced(marginX, marginY));
            dopplerButton.setBounds(bottomRow.reduced(marginX, marginY));
        }

        juce::Slider toneSlider{juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow};
        juce::Slider noiseSlider{juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxBelow};
        juce::Slider widthSlider{juce::Slider::SliderStyle::TwoValueHorizontal, juce::Slider::NoTextBox};

        juce::Label toneLabel, noiseLabel, widthLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment, noiseAttachment;
        std::unique_ptr<jr::MirrorSliderAttachment> widthAttachment;

        juce::ToggleButton dopplerButton{"Doppler On/Off"};

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> dopplerAttachment;

    private:
        AudioPluginAudioProcessor &processorRef;
    };
}