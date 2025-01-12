#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace jr
{
    class PresetManager
    {
    public:
        static const juce::File defaultDirectory;
        static const juce::String extension;

        PresetManager(juce::AudioProcessorValueTreeState &_apvts);

        void savePreset(const juce::String &presetName);
        void deletePreset(const juce::String &presetName);
        void loadPreset(const juce::String &presetName);

    private:
        juce::File getPresetFile(const juce::String &presetName);

        juce::AudioProcessorValueTreeState &apvts;
        juce::String currentPreset;
    };
}
