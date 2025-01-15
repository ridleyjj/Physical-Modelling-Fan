#include "PhysicalModellingFan/PluginProcessor.h"
#include "PhysicalModellingFan/PluginEditor.h"
#include <memory>
#include <PhysicalModellingFan/utils/jr_juceUtils.h>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), presetPanel(p.getPresetManager()), fanControls(p)
{

    fanControls.init();

    jr::JuceUtils::initSimpleSlider(this, &gainSlider, &gainLabel, "Gain");
    jr::JuceUtils::initSimpleSlider(this, &speedSlider, &speedLabel, "Speed");
    jr::JuceUtils::initSimpleSlider(this, &powerUpTimeSlider, &powerUpTimeLabel, "Power Up Time (s)");
    jr::JuceUtils::initSimpleSlider(this, &powerDownTimeSlider, &powerDownTimeLabel, "Power Down Time (s)");

    gainAttachment = createSliderAttachment(ID::GAIN, gainSlider);
    speedAttachment = createSliderAttachment(ID::SPEED, speedSlider);
    powerUpTimeAttachment = createSliderAttachment(ID::POWER_UP_T, powerUpTimeSlider);
    powerDownTimeAttachment = createSliderAttachment(ID::POWER_DOWN_T, powerDownTimeSlider);

    addAndMakeVisible(powerButton);

    powerButtonAttachment = createButtonAttachment(ID::POWER, powerButton);

    addAndMakeVisible(presetPanel);
    addAndMakeVisible(fanControls);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(800, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> AudioPluginAudioProcessorEditor::createSliderAttachment(juce::String id, juce::Slider &slider)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), id, slider);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> AudioPluginAudioProcessorEditor::createButtonAttachment(juce::String id, juce::Button &button)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), id, button);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{

    // top presets row
    presetPanel.setBoundsRelative(0.0f, 0.0f, 1.0f, 0.1f);

    // top controls horizontal row
    const auto topRowHeight = 0.2f;
    gainSlider.setBoundsRelative(0.1f, 0.1f, 0.15f, topRowHeight);
    powerButton.setBoundsRelative(0.4f, 0.1f, 0.2f, topRowHeight);

    // parallel vertical sliders
    auto numSliders = 3.0f;
    auto fullWidth = 0.45f;
    auto sliderWidth = fullWidth / numSliders;
    auto startX = fullWidth + 0.1f;

    speedSlider.setBoundsRelative(startX, 0.35f, sliderWidth, 0.6f);
    //  envelope
    powerUpTimeSlider.setBoundsRelative(startX + (sliderWidth), 0.35f, sliderWidth, 0.6f);
    powerDownTimeSlider.setBoundsRelative(startX + (sliderWidth * 2.0f), 0.35f, sliderWidth, 0.6f);

    fanControls.setBoundsRelative(0.0f, 0.35f, 0.5f, 0.65f);
}
