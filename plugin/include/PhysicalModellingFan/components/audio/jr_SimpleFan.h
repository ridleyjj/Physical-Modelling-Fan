/*
  ==============================================================================

    jr_SimpleFan.h
    Created: 26 Apr 2022 10:49:25am
    Author:  ridle

  ==============================================================================
*/

#pragma once

#include <PhysicalModellingFan/components/audio/jr_PolyBLEP_Oscillators.h> // used for jr::polyblepOscillator class
#include <PhysicalModellingFan/components/audio/jr_Delay.h>                // used for FractionalDelay class
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

namespace jr
{

    /** A class that models the toned component of a simple Propeller Fan Physical Model.
    Use setSampleRate() before use. Call process() each sample to get audio out.
    */
    class FanToneComponent
    {
    public:
        FanToneComponent() { sineOsc.setMuted(false); }

        //================================= mutator ===================================//

        /** Sets the sample rate
         * @param sr - sample rate, Hz
         */
        void setSampleRate(float sr) { sineOsc.setSampleRate(sr); }

        /** Sets the speed of the fan in Hz
         * @param frequency - speed in Hz
         */
        void setSpeed(float frequency) { sineOsc.setFrequency(frequency); }

        /** Sets the phase shift of the component, used to stagger the phase of mutliple instances of the component
         * @param shiftAmount - phase shift amount (0-0.5)
         */
        void setPhaseShift(float shiftAmount) { sineOsc.setPhaseShift(shiftAmount); }

        /** Sets the pulse width of the component
         * @param pw - pulse width
         */
        void setPulseWidth(float pw)
        {
            if (pw > 0)
                pulseWidth = pw;
        }

        /** Sets the volume level of the tone component
         * @param vol - volume level (0-1)
         */
        void setLevel(float vol)
        {
            if (vol >= 0 && vol <= 1.0)
                level = vol;
        }

        //================================= accessor ===================================//

        /** returns the current sample value for the raw sine wave before it has been transformed into the tone, used to control other connected components
         * @return rawSineSignal - current sample value of raw sine signal
         */
        float getRawSine() { return rawSineSignal; }

        /** returns the current sample value for the output audio signal before the volume level has been applied, used to send to noise component
         * @return rawSignal - current sample value for the raw audio output signal
         */
        float getRawSignal() { return rawSignal; }

        /** Processes the tone component and returns the next sample value for the audio signal
         * @return sampleOut - next sample value for audio signal out
         */
        float process();

    private:
        polyblepOscillator sineOsc; // sine oscillator used as base of the tone component
        float phaseShift{};         // amount of phase shift (0-0.5), used to stagger phase of multiple instances
        float pulseWidth{8.0};      // pulse width of waveform
        float level{1.0f};          // volume level of tone component (0-1)
        float rawSineSignal{};      // current sample value for the raw sine signal, used to control delay or doppler components that may be connected
        float rawSignal{};          // current sample value for the output audio signal before the volume level has been applied, used to send to an attached noise component
    };

    /** A class that models the noise component of a simple Propeller Fan Physical Model.
    Use setSampleRate() before use. Call process() each sample to get audio out.
    */
    class FanNoiseComponent
    {
    public:
        //================================= mutator ===================================//

        /** Sets the sample rate of the component
         * @param sr - sample rate (Hz)
         */
        void setSampleRate(float sr) { sampleRate = sr; }

        /** Sets the volume level of the component
         * @param gain - volume level (0-1)
         */
        void setLevel(float gain) { level = gain; }

        /** Sets the parameters of the filter
         * @param freq - cutoff frequency (Hz)
         * @param q - resonance value
         */
        void setFilterParams(float freq, float q);

        /** Sets the filter type
         * @param typeIndex - filter type (0=BandPass, 1=LowPass)
         */
        void setFilterType(size_t typeIndex)
        {
            if (typeIndex == 0 || typeIndex == 1)
                filterType = typeIndex;
        }

        //================================= accessor ===================================//

        /** returns the next sample value for the noise component
         * @param rawSignalIn - raw signal from attached tone component
         * @return sampleOut - next sample value
         */
        virtual float process(float rawSignalIn);

    protected:
        float cutoff{700.0f};   // cutoff frequency of filter (Hz)
        float resonance{1.0f};  // resonance (Q value) of filter
        juce::IIRFilter filter; // filter
        float sampleRate{};     // sample rate of component (Hz)
        juce::Random random;    // random number generator for white noise
        float level{1.0f};      // volume level of nosie component (0-1)
        size_t filterType{};    // filter type index (0=BandPass, 1=LowPass)
    };

    /** A type of noise component class for a simple fan, where a doppler effect is created with the filter using a control signal
    Use setSampleRate() before use. Call process() each sample to get audio out. setDoppler() turns doppler on or off.
    setFilterParams() can be used to set the parameters for the noise component when dopper is turned OFF, for filter parameters that will be controlled by doppler use setDopplerParams()
    */
    class FanDopplerComponent : public FanNoiseComponent
    {
    public:
        /** Sets the parameters for the doppler processed signal according to the cutoff range and a control signal use to modulate the cutoff frequency
         * @param controlSignalIn - current sample value for control signal
         * @param range - range to be modulated over cutoff frequency
         * @param offset - offset of cutoff frequency
         * @param q - resonance value for filter
         */
        void setDopplerParams(float controlSignalIn, float range, float offset, float q);

        void setDopplerParams(float controlSignalIn) { setDopplerParams(controlSignalIn, cutoffRange, cutoffOffset, dopplerRes); }

        /** Sets whether or not doppler effect is processed or not
         * bool isOn - true to turn doppler effect on, false to turn off
         */
        void setDopplerOn(bool isOn) { dopplerOn = isOn; }

        /** Returns the next sample value for the noise component - affected by doppler affect if doppler is on, and not if it is off
         * @param rawSignalIn - raw signal from attached tone component
         * @return sampleOut - next sample value
         */
        float process(float rawSignalIn) override;

    private:
        float cutoffRange{500.0f};   // range of modulation of cutoff frequency (Hz)
        float cutoffOffset{100.0f};  // offset of cutoff frequency (Hz)
        float dopplerCutoff{700.0f}; // current cutoff frequency resulting from doppler modulation (Hz)
        float dopplerRes{5.0f};      // current resonance value for filter with doppler effect
        bool dopplerOn{true};        // doppler effect on/off
    };

    /** A specific delay class used to create a fast blade effect for a Fan Physical Model by varying the delay length of a delay line at a set rate
    Use setSampleRate() before use. Call process() each sample for output.
    */
    class FanDelay
    {
    public:
        /** Sets the sample rate and initialises the delay
         * @param sr - sample rate (Hz)
         */
        void setSampleRate(float sr);

        /** Sets the amount of 'chop' to the fan blades, which is the modulation depth of the delay time in ms
         * @param chopIn - chop value (ms)
         */
        void setChop(float chopIn)
        {
            if (chopIn >= 0 && chopIn <= 99.9)
                chop = chopIn;
        }

        /** processes the new delay length according to the control signal, and then processes the audioSignalIn, returning a mix of the dry and delayed signal
         * @param controlSignalIn - current sample value for the control signal
         * @param audioSignalIn - current sample value for the dry audio signal
         */
        float process(float controlSignalIn, float audioSignalIn);

    private:
        float chop{10.0f};         // modulation depth of the delay length in ms (0-99.9)
        float sampleRate{};        // sample rate, Hz
        FractionalDelay delayLine; // delay line
    };

    /** A simple stereo panner class that takes a signal value in and uses it to oscillate panning position around centre to a set pan width amount
    Use setPanWidth() before use. Call process() each sample to calculate new pan values, and then use getLeft() and getRight() to access volume levels for each channel.
    */
    class FanPanner
    {
    public:
        /** Sets the depth of the pan modulation around centre
         * @param width - pan width/depth (0-1)
         */
        void setPanWidth(float width)
        {
            if (width >= 0 && width <= 1)
                panWidth = width;
        }

        /** calculates new pan values for stereo channels using an input current sample value of a control signal
         * @param controlSignalIn - current sample value for control signal
         */
        void process(float controlSignalIn);

        /** Returns the volume level for the left channel
         * @param leftLevel - volume level for left channel (0-1)
         */
        float getLeft() { return leftLevel; }

        /** Returns the volume level for the right channel
         * @param rightLevel - volume level for right channel (0-1)
         */
        float getRight() { return rightLevel; }

    private:
        float panWidth{};   // width/depth of panning modulation around centre (0-1)
        float leftLevel{};  // volume level for left channel
        float rightLevel{}; // volume level for right channel
    };

    class MainBlades
    {
    public:
        void setLevel(float vol) { level = vol; }

        void setSampleRate(float _sampleRate);

        /** sets the volume value for the tone component of the main blades
         * @param vol - volume level (0-1)
         */
        void setToneLevel(float vol) { toneComp.setLevel(vol); }

        /** Sets the volume value for the noise component of the main blades
         * @param vol - volume level (0-1)
         */
        void setNoiseLevel(float vol) { noiseComp.setLevel(vol); }

        /** Sets the doppler effect on or off for the cutoff frequency of the main blades noise component
         * @param isOn - true to turn doppler effect on, false to turn off
         */
        void setDopplerOn(bool isOn) { noiseComp.setDopplerOn(isOn); }

        /** Updates the doppler effected noise components cutoff frequency value using the raw sine signal from the main blades tone comp as a control signal
         * @return
         */
        void setDopplerParams() { noiseComp.setDopplerParams(toneComp.getRawSine()); }

        void setSpeed(float speedInHz) { toneComp.setSpeed(speedInHz); }

        void setPulseWidth(float pw) { toneComp.setPulseWidth(pw); }

        /** processes the next mono sample value for the main blades
         */
        float process();

        /** returns the raw signal value from the tone component to be used for controlling a panning component
         */
        float getPanControlSignal() { return toneComp.getRawSine(); }

    private:
        float level{1.0f};
        FanToneComponent toneComp{};     // tone component of main blades
        FanDopplerComponent noiseComp{}; // noise component of main blades with doppler capabilities
    };

    class FastBlades
    {
    public:
        FastBlades();

        void setSampleRate(float _sampleRate);

        float process();

        /** Sets the chop value for the delay component, which is the modulation depth of the delay length
         * @param chop - modulation depth of the delay length (ms)
         */
        void setChop(float chop) { delayComp.setChop(chop); }

        void setLevel(float vol) { level = vol; }

        /** sets the volume value for the tone component of the fast blades
         * @param vol - volume level (0-1)
         */
        void setToneLevel(float vol) { toneComp.setLevel(vol); }

        /** Sets the volume value for the noise component of the fast blades
         * @param vol - volume level (0-1)
         */
        void setNoiseLevel(float vol) { noiseComp.setLevel(vol); }

        /** Sets the speed of the fan in Hz
         * @param speedInHz
         */
        void setSpeed(float speedInHz) { toneComp.setSpeed(speedInHz); }

        /** Sets the pulse width of the tone components
         * @param pw - pulse width
         */
        void setPulseWidth(float pw) { toneComp.setPulseWidth(pw); }

    private:
        float level{0.65f};
        FanToneComponent toneComp{};   // tone component of fast blades
        FanNoiseComponent noiseComp{}; // noise component of fast blades
        FanDelay delayComp{};          // delay component of fast blades
    };

    class FanPropeller
    {
    public:
        FanPropeller() {}

        //============================ mutators ============================//

        /** Sets the sample rate
         * @param sr - sample rate (Hz)
         */
        void setSampleRate(float sr);

        /** Sets the max speed of the fan in Hz
         * @param speedInHz
         */
        void setSpeed(float speedInHz)
        {
            maxSpeed = speedInHz;
        };

        /** Sets the pulse width of the tone components
         * @param pw - pulse width
         */
        void setPulseWidth(float pw);

        /** Sets the depth of modulation of the pan position from centre
         * @param width - modulation depth of pan from centre (0-1)
         */
        void setPanWidth(float width) { pannerComp.setPanWidth(width); }

        /** Sets the chop value for the delay component, which is the modulation depth of the delay length
         * @param chop - modulation depth of the delay length (ms)
         */
        void setChop(float chop) { fastBlades.setChop(chop); }

        /** Sets the doppler effect on or off for the cutoff frequency of the main blades noise component
         * @param isOn - true to turn doppler effect on, false to turn off
         */
        void setDopplerOn(bool isOn) { mainBlades.setDopplerOn(isOn); }

        /** Updates the doppler effected noise components cutoff frequency value using the raw sine signal from the main blades tone comp as a control signal
         * @return
         */
        void setDopplerParams() { mainBlades.setDopplerParams(); }

        void setMainBladesLevel(float vol) { mainBlades.setLevel(vol); }
        void setFastBladesLevel(float vol) { fastBlades.setLevel(vol); }

        void setToneLevel(float toneLevel);

        void setNoiseLevel(float noiseLevel);

        /** sets the volume value for the tone component of the main blades
         * @param vol - volume level (0-1)
         */
        void setMainToneLevel(float vol) { mainBlades.setToneLevel(vol); }

        /** sets the volume value for the tone component of the fast blades
         * @param vol - volume level (0-1)
         */
        void setFastToneLevel(float vol) { fastBlades.setToneLevel(vol); }

        /** Sets the volume value for the noise component of the main blades
         * @param vol - volume level (0-1)
         */
        void setMainNoiseLevel(float vol) { mainBlades.setNoiseLevel(vol); }

        /** Sets the volume value for the noise component of the fast blades
         * @param vol - volume level (0-1)
         */
        void setFastNoiseLevel(float vol) { fastBlades.setNoiseLevel(vol); }

        /** processes the next sample values for the fans left and right channels
         */
        void process(float envelope);

        //============================ accessors ============================//

        /** returns the current sample value for the left channel of the fan
         * @return sampleOut
         */
        float getLeftSample() { return currentLeftSample; }

        /** returns the current sample value for the right channel of the fan
         * @return sampleOut
         */
        float getRightSample() { return currentRightSample; }

    private:
        /** Sets the current speed of the fan tone and noise components in Hz
        Used to set the current value based on the maxSpeed and current envelope value
        * @param speedInHz
        */
        void setCurrentSpeed(float speedInHz);

        FanPanner pannerComp{}; // panning component for whole system (controlled by main blades)
        MainBlades mainBlades{};
        FastBlades fastBlades{};

        //============ params ============//

        bool hasInit{false};

        float maxSpeed{};           // max speed in Hz
        float currentLeftSample{};  // current sample value for left channel
        float currentRightSample{}; // current sample value for right channel
    };
}