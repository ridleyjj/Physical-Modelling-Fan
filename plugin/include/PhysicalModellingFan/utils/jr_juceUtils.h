#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace jr
{
    class JuceUtils
    {
    public:
        static void initSimpleSlider(juce::Component *parent, juce::Slider *slider, juce::Label *label, const juce::String &name)
        {
            // slider init
            slider->setTextBoxIsEditable(false);
            parent->addAndMakeVisible(slider);

            // text label init
            label->setText(name, juce::NotificationType::dontSendNotification);
            label->setJustificationType(juce::Justification::centredTop);
            label->attachToComponent(slider, false);
        }

        static void initSimpleSliderWithRange(juce::Component *parent, juce::Slider *slider, juce::Label *label, const juce::String &name, double minVal, double maxVal, double step)
        {
            initSimpleSlider(parent, slider, label, name);

            slider->setRange(minVal, maxVal, step);
        }
    };
}