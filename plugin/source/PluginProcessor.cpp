#include "PhysicalModellingFan/PluginProcessor.h"
#include "PhysicalModellingFan/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    apvts.state.setProperty(jr::PresetManager::presetNameProperty, "", nullptr);

    apvts.addParameterListener(ID::GAIN, &masterGainListener);
    apvts.addParameterListener(ID::SPEED, &speedListener);
    apvts.addParameterListener(ID::FAN_TONE, &fanToneLevelListener);
    apvts.addParameterListener(ID::FAN_NOISE, &fanNoiseLevelListener);
    apvts.addParameterListener(ID::FAN_WIDTH, &fanStereoWidthListener);
    apvts.addParameterListener(ID::FAN_DOPPLER, &fanDopplerToggleListener);
    apvts.addParameterListener(ID::POWER, &powerToggleListener);
    apvts.addParameterListener(ID::POWER_UP_T, &powerUpTimeListener);
    apvts.addParameterListener(ID::POWER_DOWN_T, &powerDownTimeListener);

    presetManager = std::make_unique<jr::PresetManager>(apvts);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    apvts.removeParameterListener(ID::GAIN, &masterGainListener);
    apvts.removeParameterListener(ID::SPEED, &speedListener);
    apvts.removeParameterListener(ID::FAN_TONE, &fanToneLevelListener);
    apvts.removeParameterListener(ID::FAN_NOISE, &fanNoiseLevelListener);
    apvts.removeParameterListener(ID::FAN_WIDTH, &fanStereoWidthListener);
    apvts.removeParameterListener(ID::FAN_DOPPLER, &fanDopplerToggleListener);
    apvts.removeParameterListener(ID::POWER, &powerToggleListener);
    apvts.removeParameterListener(ID::POWER_UP_T, &powerUpTimeListener);
    apvts.removeParameterListener(ID::POWER_DOWN_T, &powerDownTimeListener);
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    machine.setSampleRate((float)sampleRate);
    machine.setSpeed(*apvts.getRawParameterValue(ID::SPEED));
    machine.setFanDoppler(*apvts.getRawParameterValue(ID::FAN_DOPPLER));
    machine.setFanLevel(*apvts.getRawParameterValue(ID::GAIN));
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float *leftChannel = buffer.getWritePointer(0);
    float *rightChannel = buffer.getWritePointer(1);
    int numSamples = buffer.getNumSamples();

    float gainVal = 0.4f;

    //=============================== DSP LOOP ===============================//
    for (int i = 0; i < numSamples; i++)
    {
        machine.process();

        leftChannel[i] = gainVal * machine.getCurrentSampleLeft();
        rightChannel[i] = gainVal * machine.getCurrentSampleRight();
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

//==============================================================================
// Creates parameters for APVTS
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::GAIN, "Gain", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::SPEED, "Speed", 1.0f, 10.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::FAN_TONE, "Tone Level", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::FAN_NOISE, "Noise Level", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::FAN_WIDTH, "Stereo Width", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>(ID::FAN_DOPPLER, "Doppler On/Off", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(ID::POWER, "Power On/Off", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::POWER_UP_T, "Power Up Time (s)", 0.1f, 8.0f, 1.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::POWER_DOWN_T, "Power Down Time (s)", 0.1f, 8.0f, 1.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ID::ACCEL_RATE, "Acceleration Rate", 0.0f, 1.0f, 0.5f));

    return layout;
}