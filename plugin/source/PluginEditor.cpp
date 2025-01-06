#include "PhysicalModellingFan/PluginProcessor.h"
#include "PhysicalModellingFan/PluginEditor.h"
#include <memory>

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    initSimpleSlider(&gainSlider, &gainLabel, "Gain");
    initSimpleSlider(&speedSlider, &speedLabel, "Speed");
    initSimpleSlider(&fanToneSlider, &fanToneLabel, "Fan Tone");
    initSimpleSlider(&fanNoiseSlider, &fanNoiseLabel, "Fan Noise");
    initSimpleSliderWithRange(&fanWidthSlider, &fanWidthLabel, "Fan Stereo With", -1.0, 1.0, 0.01);

    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::GAIN, gainSlider);
    speedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::SPEED, speedSlider);
    fanToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::FAN_TONE, fanToneSlider);
    fanNoiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.getAPVTS(), ID::FAN_NOISE, fanNoiseSlider);
    fanWidthAttachment = std::make_unique<jr::MirrorSliderAttachment>(*(processorRef.getAPVTS().getParameter(ID::FAN_WIDTH)), fanWidthSlider);

    addAndMakeVisible(fanDopplerButton);

    dopplerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.getAPVTS(), ID::FAN_DOPPLER, fanDopplerButton);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::initSimpleSlider(juce::Slider* slider, juce::Label* label, const juce::String& name) {
    // slider init
    slider->setTextBoxIsEditable(false);
    addAndMakeVisible(slider);

    // text label init
    label->setText(name, juce::NotificationType::dontSendNotification);
    label->setJustificationType(juce::Justification::centredTop);
    label->attachToComponent(slider, false);
}

void AudioPluginAudioProcessorEditor::initSimpleSliderWithRange(juce::Slider* slider, juce::Label* label, const juce::String& name, double minVal, double maxVal, double step) {
    initSimpleSlider(slider, label, name);

    slider->setRange(minVal, maxVal, step);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // top horizontal row
    gainSlider.setBoundsRelative(0.1f, 0.1f, 0.2f, 0.2f);
    fanDopplerButton.setBoundsRelative(0.7f, 0.1f, 0.2f, 0.2f);

    // parallel vertical sliders
    speedSlider.setBoundsRelative(0.1f, 0.35f, 0.2f, 0.35f);
    fanToneSlider.setBoundsRelative(0.4f, 0.35f, 0.2f, 0.35f);
    fanNoiseSlider.setBoundsRelative(0.7f, 0.35f, 0.2f, 0.35f);
    
    // bottom horizontal row
    fanWidthSlider.setBoundsRelative(0.1f, 0.8f, 0.8f, 0.2f);
}
