/*
  ==============================================================================

    jr_SimpleFan.cpp
    Created: 26 Apr 2022 10:52:41am
    Author:  ridle

  ==============================================================================
*/

#include <PhysicalModellingFan/components/audio/jr_SimpleFan.h>

namespace jr
{

    //======================= Tone Component =========================//

    float FanToneComponent::process()
    {
        rawSineSignal = sineOsc.processSingleSample();

        // waveshaping technique of 1/(1 + x^2) used to obtain narrow pulse wave
        rawSignal = 1.0f / (1.0f + pow(rawSineSignal * pulseWidth, 2));

        return rawSignal * level;
    }

    //======================= Noise Component =========================//

    void FanNoiseComponent::setFilterParams (float freq, float q)
    {
        if (freq > 0)
            cutoff = freq;

        if (q > 0)
            resonance = q;
    }

    float FanNoiseComponent::process (float rawSignalIn)
    {
        switch (filterType)
        {
        default:
            filter.setCoefficients (juce::IIRCoefficients::makeBandPass(sampleRate, cutoff, resonance));
            break;
        case 1:
            filter.setCoefficients (juce::IIRCoefficients::makeLowPass(sampleRate, cutoff, resonance));
            break;
        }

        float filteredNoise = filter.processSingleSampleRaw (random.nextFloat());

        float sampleOut = filteredNoise * rawSignalIn;

        return sampleOut * level;
    }

    //======================= Panner Component =========================//

    void FanPanner::process (float controlSignalIn)
    {
        rightLevel = (((controlSignalIn + 1.0f) / 2.0f) * panWidth) + 0.5f - (panWidth / 2.0f);

        leftLevel = 1.0f - rightLevel;
    }

    //======================= Doppler Component =========================//

    void FanDopplerComponent::setDopplerParams (float controlSignalIn, float range, float offset, float q)
    {
        cutoffRange = range;
        cutoffOffset = offset;

        if (q > 0)
            dopplerRes = q;

        dopplerCutoff = offset + (((controlSignalIn + 1.0f) / 2.0f) * range);

        if (dopplerCutoff < 0)
            dopplerCutoff = 0;
    }

    float FanDopplerComponent::process (float rawSignalIn)
    {
        if (dopplerOn)
        {
            switch (filterType)
            {
            default:
                filter.setCoefficients (juce::IIRCoefficients::makeBandPass (sampleRate, dopplerCutoff, dopplerRes));
                break;
            case 1:
                filter.setCoefficients (juce::IIRCoefficients::makeLowPass (sampleRate, dopplerCutoff, dopplerRes));
                break;
            }

            float filteredNoise = filter.processSingleSampleRaw (random.nextFloat());

            float sampleOut = filteredNoise * rawSignalIn;

            return sampleOut * level;
        }
        else
        {
            return FanNoiseComponent::process (rawSignalIn);
        }
    }

    //======================= Delay Component =========================//

    void FanDelay::setSampleRate (float sr)
    {
        sampleRate = sr;

        delayLine.setSampleRate (sampleRate);
        delayLine.setSize (0.4);
    }

    float FanDelay::process (float controlSignalIn, float audioSignalIn)
    {
        float delayTimeInMs = 200 + (controlSignalIn * chop);

        delayLine.setDelayTime (delayTimeInMs / 1000.0f);

        return delayLine.process (audioSignalIn);
    }

    //======================== Main Blades ==========================//

    void MainBlades::setSampleRate(float _sampleRate)
    {
        toneComp.setSampleRate(_sampleRate);
        noiseComp.setSampleRate(_sampleRate);
    }

    float MainBlades::process()
    {
        float toneOut = toneComp.process();
        setDopplerParams();
        return level * (toneOut + noiseComp.process (toneComp.getRawSignal()));
    }

    //======================== Fast Blades ==========================//

    FastBlades::FastBlades()
    {
        noiseComp.setFilterType (1);
        toneComp.setPhaseShift (0.25);
    }

    void FastBlades::setSampleRate(float _sampleRate)
    {
        toneComp.setSampleRate (_sampleRate);
        noiseComp.setSampleRate (_sampleRate);
        delayComp.setSampleRate (_sampleRate);
    }

    float FastBlades::process()
    {
        float toneOut = toneComp.process();
        float noiseOut = noiseComp.process (toneComp.getRawSignal());
        return level * (toneOut + delayComp.process(toneComp.getRawSine(), noiseOut));
    }

    //======================= Fan Propeller =========================//

    void FanPropeller::setMappedParams (float gainIn, float speedIn, float toneLevelIn, float noiseLevelIn, float stereoWidthIn, bool dopplerOnIn)
    {
        setParams (speedIn, gainIn, 1.0f, 1.0f, toneLevelIn, noiseLevelIn, toneLevelIn, noiseLevelIn, dopplerOnIn, 10.0f, stereoWidthIn);
    }

    void FanPropeller::setSampleRate (float sr)
    {
        mainBlades.setSampleRate(sr);
        fastBlades.setSampleRate (sr);
    }

    void FanPropeller::setSpeed (float speedInHz)
    {
        mainBlades.setSpeed(speedInHz);
        fastBlades.setSpeed (speedInHz);
    }

    void FanPropeller::setPulseWidth (float pw)
    {
        mainBlades.setPulseWidth (pw);
        fastBlades.setPulseWidth (pw);
    }

    void FanPropeller::setParams (float speedInHz, float masterVol, float mainBladesLevelIn, float fastBladesLevelIn, float mainBladesToneLevel, float mainBladesNoiseLevel, float fastBladesToneLevel, float fastBladesNoiseLevel, bool dopplerOn, float chopIn, float panWidthIn)
    {
        setSpeed (speedInHz);
        setLevel (masterVol);
        setMainBladesLevel (mainBladesLevelIn);
        setFastBladesLevel (fastBladesLevelIn);
        setMainToneLevel (mainBladesToneLevel);
        setFastToneLevel (fastBladesToneLevel);
        setMainNoiseLevel (mainBladesNoiseLevel);
        setFastNoiseLevel (fastBladesNoiseLevel);
        setDopplerOn (dopplerOn);
        setChop (chopIn);
        setPanWidth (panWidthIn);
    }

    void FanPropeller::process()
    {
        float rawOut = level * (fastBlades.process() + mainBlades.process());

        pannerComp.process (mainBlades.getPanControlSignal());

        currentLeftSample = rawOut * pannerComp.getLeft();
        currentRightSample = rawOut * pannerComp.getRight();
    }
}
