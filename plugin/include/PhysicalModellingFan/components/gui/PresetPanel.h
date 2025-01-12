#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace jr
{
    class PresetPanel : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
    {
    public:
        PresetPanel()
        {
            configureButton(saveButton, "Save");
            configureButton(deleteButton, "Delete");

            presetList.setTextWhenNothingSelected("No Preset Selected");
            presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
            addAndMakeVisible(presetList);
            presetList.addListener(this);
        }

        ~PresetPanel()
        {
            saveButton.removeListener(this);
            deleteButton.removeListener(this);
            presetList.removeListener(this);
        }

        void resized() override
        {
            const int margin = 4;
            const auto container = getLocalBounds().reduced(margin);
            auto bounds = container;

            deleteButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(margin));
            presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.6f)).reduced(margin));
            saveButton.setBounds(bounds.reduced(margin));
        }

    private:
        void buttonClicked(juce::Button *button) override {};
        void comboBoxChanged(juce::ComboBox *comboBox) override {};

        void configureButton(juce::Button &button, const juce::String &buttonText)
        {
            button.setButtonText(buttonText);
            button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
            addAndMakeVisible(button);
            button.addListener(this);
        }

        juce::TextButton saveButton, deleteButton;
        juce::ComboBox presetList;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel);
    };
}