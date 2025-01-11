#include <PhysicalModellingFan/components/services/jr_PresetManager.h>

namespace jr
{
    const juce::File PresetManager::defaultDirectory{
        juce::File::getSpecialLocation(
            juce::File::SpecialLocationType::commonDocumentsDirectory)
            .getChildFile(COMPANY_NAME)
            .getChildFile(PROJECT_NAME)};
    const juce::String PresetManager::extension{"preset"};

    PresetManager::PresetManager(juce::AudioProcessorValueTreeState &_apvts) : apvts(_apvts)
    {
        if (!defaultDirectory.exists())
        {
            const auto result = defaultDirectory.createDirectory();
            if (result.failed())
            {
                DBG("Could not create preset directory: " + result.getErrorMessage());
                jassertfalse;
            }
        }
    }

    void PresetManager::savePreset(const juce::String &presetName)
    {
        if (presetName.isEmpty())
            return;

        const auto xmlState = apvts.copyState().createXml();
        const auto presetFile = getPresetFile(presetName);
        if (!xmlState->writeTo(presetFile))
        {
            DBG("Could not create preset file: " + presetFile.getFullPathName());
            jassertfalse;
        }
        currentPreset = presetName;
    }

    void PresetManager::deletePreset(const juce::String &presetName)
    {
        if (presetName.isEmpty())
            return;

        const auto presetFile = getPresetFile(presetName);
        if (!presetFile.existsAsFile())
        {
            DBG("Preset file: " + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
            return;
        }
        if (!presetFile.deleteFile())
        {
            DBG("Preset file: " + presetFile.getFullPathName() + " could not be deleted");
            jassertfalse;
            return;
        }
        currentPreset = "";
    }

    void PresetManager::loadPreset(const juce::String &presetName)
    {
        if (presetName.isEmpty())
            return;

        const auto presetFile = getPresetFile(presetName);
        if (!presetFile.existsAsFile())
        {
            DBG("Preset file: " + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
            return;
        }
    }

    //================= private methods =====================

    juce::File PresetManager::getPresetFile(const juce::String &presetName)
    {
        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        if (!presetFile.existsAsFile())
        {
            DBG("Preset file: " + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
        }
    }
}