#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <PhysicalModellingFan/components/audio/jr_Machine.h>
#include <PhysicalModellingFan/components/audio/ApvtsListener.h>

namespace ID
{
    const juce::String GAIN = "GAIN";
    const juce::String SPEED = "SPEED";
    const juce::String FAN_TONE = "FAN_TONE";
    const juce::String FAN_NOISE = "FAN_NOISE";
    const juce::String FAN_WIDTH = "FAN_WIDTH";
    const juce::String FAN_DOPPLER = "FAN_DOPPLER";
    const juce::String POWER = "POWER";
    const juce::String POWER_UP_T = "POWER_UP_T";
    const juce::String POWER_DOWN_T = "POWER_DOWN_T";
    const juce::String ACCEL_RATE = "ACCEL_RATE";
}

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    //==============================================================================

    // shared
    void togglePower(bool powerOn) { machine.togglePower(powerOn); }
    void setSpeed(float speed) { machine.setSpeed(speed); }

    // envelope
    void setPowerUpTime(float seconds) { machine.setPowerUpTime(seconds); }
    void setPowerDownTime(float seconds) { machine.setPowerDownTime(seconds); }

    // fan
    void setMasterGain(float gain) { machine.setFanLevel(gain); }
    void setFanToneLevel(float level) { machine.setFanToneLevel(level); }
    void setFanNoiseLevel(float level) { machine.setFanNoiseLevel(level); }
    void setFanStereoWidth(float width) { machine.setFanStereoWidth(width); }
    void setFanDoppler(bool isOn) { machine.setFanDoppler(isOn); }

    juce::AudioProcessorValueTreeState &getAPVTS() { return apvts; }

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)

    jr::Machine machine{};

    juce::AudioProcessorValueTreeState apvts;

    jr::ApvtsListener masterGainListener{[&](float newValue)
                                         { setMasterGain(newValue); }};
    jr::ApvtsListener speedListener{[&](float newValue)
                                    { setSpeed(newValue); }};
    jr::ApvtsListener fanToneLevelListener{[&](float newValue)
                                           { setFanToneLevel(newValue); }};
    jr::ApvtsListener fanNoiseLevelListener{[&](float newValue)
                                            { setFanNoiseLevel(newValue); }};
    jr::ApvtsListener fanStereoWidthListener{[&](float newValue)
                                             { setFanStereoWidth(newValue); }};
    jr::ApvtsListener fanDopplerToggleListener{[&](bool newValue)
                                               { setFanDoppler(newValue); }};
    jr::ApvtsListener powerToggleListener{[&](bool newValue)
                                          { togglePower(newValue); }};
    jr::ApvtsListener powerUpTimeListener{[&](float newValue)
                                          { setPowerUpTime(newValue); }};
    jr::ApvtsListener powerDownTimeListener{[&](float newValue)
                                            { setPowerDownTime(newValue); }};
};
