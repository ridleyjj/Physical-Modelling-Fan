#include "PhysicalModellingFan/PluginProcessor.h"
#include "PhysicalModellingFan/PluginEditor.h"
#include <memory>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    juce::ignoreUnused(processorRef);

    initSimpleSlider(&gainSlider, &gainLabel, "Gain");
    initSimpleSlider(&speedSlider, &speedLabel, "Speed");
    initSimpleSlider(&fanToneSlider, &fanToneLabel, "Fan Tone");
    initSimpleSlider(&fanNoiseSlider, &fanNoiseLabel, "Fan Noise");
    initSimpleSliderWithRange(&fanWidthSlider, &fanWidthLabel, "Fan Stereo With", -1.0, 1.0, 0.01);
    initSimpleSlider(&powerUpTimeSlider, &powerUpTimeLabel, "Power Up Time (s)");
    initSimpleSlider(&powerDownTimeSlider, &powerDownTimeLabel, "Power Down Time (s)");

    gainAttachment = createSliderAttachment(ID::GAIN, gainSlider);
    speedAttachment = createSliderAttachment(ID::SPEED, speedSlider);
    fanToneAttachment = createSliderAttachment(ID::FAN_TONE, fanToneSlider);
    fanNoiseAttachment = createSliderAttachment(ID::FAN_NOISE, fanNoiseSlider);
    fanWidthAttachment = std::make_unique<jr::MirrorSliderAttachment>(*(processorRef.getAPVTS().getParameter(ID::FAN_WIDTH)), fanWidthSlider);
    powerUpTimeAttachment = createSliderAttachment(ID::POWER_UP_T, powerUpTimeSlider);
    powerDownTimeAttachment = createSliderAttachment(ID::POWER_DOWN_T, powerDownTimeSlider);

    addAndMakeVisible(fanDopplerButton);
    addAndMakeVisible(powerButton);

    dopplerAttachment = createButtonAttachment(ID::FAN_DOPPLER, fanDopplerButton);
    powerButtonAttachment = createButtonAttachment(ID::POWER, powerButton);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(600, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::initSimpleSlider(juce::Slider *slider, juce::Label *label, const juce::String &name)
{
    // slider init
    slider->setTextBoxIsEditable(false);
    addAndMakeVisible(slider);

    // text label init
    label->setText(name, juce::NotificationType::dontSendNotification);
    label->setJustificationType(juce::Justification::centredTop);
    label->attachToComponent(slider, false);
}

void AudioPluginAudioProcessorEditor::initSimpleSliderWithRange(juce::Slider *slider, juce::Label *label, const juce::String &name, double minVal, double maxVal, double step)
{
    initSimpleSlider(slider, label, name);

    slider->setRange(minVal, maxVal, step);
}

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
    // top horizontal row
    gainSlider.setBoundsRelative(0.1f, 0.1f, 0.2f, 0.2f);
    powerButton.setBoundsRelative(0.4f, 0.1f, 0.2f, 0.2f);
    fanDopplerButton.setBoundsRelative(0.7f, 0.1f, 0.2f, 0.2f);

    // parallel vertical sliders
    auto numSliders = 5.0f;
    auto fullWidth = 0.95f;
    auto sliderWidth = fullWidth / numSliders;
    auto startX = (1.0f - fullWidth) / 2.0f;
    // fan
    speedSlider.setBoundsRelative(startX, 0.35f, sliderWidth, 0.35f);
    fanToneSlider.setBoundsRelative(startX + sliderWidth, 0.35f, sliderWidth, 0.35f);
    fanNoiseSlider.setBoundsRelative(startX + (sliderWidth * 2.0f), 0.35f, sliderWidth, 0.35f);
    // envelope
    powerUpTimeSlider.setBoundsRelative(startX + (sliderWidth * 3.0f), 0.35f, sliderWidth, 0.35f);
    powerDownTimeSlider.setBoundsRelative(startX + (sliderWidth * 4.0f), 0.35f, sliderWidth, 0.35f);

    // bottom horizontal row
    fanWidthSlider.setBoundsRelative(0.1f, 0.8f, 0.8f, 0.2f);
}
