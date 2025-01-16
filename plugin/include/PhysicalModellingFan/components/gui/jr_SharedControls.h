#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <PhysicalModellingFan/PluginProcessor.h>
#include <PhysicalModellingFan/utils/jr_juceUtils.h>

namespace jr
{
    /**
     * A class that contains all of the controls that are shared across components such as envelope controls and master volume.
     */
    class SharedControls : public juce::Component
    {
    public:
        SharedControls(AudioPluginAudioProcessor &p) : processorRef(p) {}

        /**
         * Initialises all sliders, buttons and their attachments.
         */
        void init()
        {
            jr::JuceUtils::initSimpleSlider(this, &gainSlider, &gainLabel, "Gain");
            jr::JuceUtils::initSimpleSlider(this, &speedSlider, &speedLabel, "Speed");
            jr::JuceUtils::initSimpleSlider(this, &powerUpTimeSlider, &powerUpTimeLabel, "Power Up Time (s)");
            jr::JuceUtils::initSimpleSlider(this, &powerDownTimeSlider, &powerDownTimeLabel, "Power Down Time (s)");
            gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::GAIN, gainSlider);
            speedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::SPEED, speedSlider);
            powerUpTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::POWER_UP_T, powerUpTimeSlider);
            powerDownTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::POWER_DOWN_T, powerDownTimeSlider);
            addAndMakeVisible(powerButton);
            powerButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ID::POWER, powerButton);
        }

        void resized() override
        {
            const int topRowMarginY = 16;
            const int margin = 8;
            const auto container = getLocalBounds().reduced(margin);
            auto bounds = container;

            auto topRow = bounds.removeFromTop(container.proportionOfHeight(0.25f)).reduced(margin, topRowMarginY);
            gainSlider.setBounds(topRow.removeFromLeft(container.proportionOfWidth(0.5f)).reduced(margin));
            powerButton.setBounds(topRow.reduced(margin));
            speedSlider.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.33f)).reduced(margin));
            powerUpTimeSlider.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.34f)).reduced(margin));
            powerDownTimeSlider.setBounds(bounds.reduced(margin));
        }

    private:
        AudioPluginAudioProcessor &processorRef;

        // Sliders

        juce::Slider speedSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
        juce::Slider powerUpTimeSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
        juce::Slider powerDownTimeSlider{juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextBoxBelow};
        juce::Slider gainSlider{juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow};

        juce::Label gainLabel, speedLabel, powerDownTimeLabel, powerUpTimeLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment, speedAttachment, powerDownTimeAttachment, powerUpTimeAttachment;

        // Buttons

        juce::ToggleButton powerButton{"Power On/Off"};

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerButtonAttachment;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SharedControls)
    };
}
