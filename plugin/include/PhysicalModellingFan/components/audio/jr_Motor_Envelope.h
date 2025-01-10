/*
  ==============================================================================

    Motor_Envelope.h
    Created: 20 Apr 2022 9:30:50am
    Author:  ridle

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace jr
{
    /**
    A class to simulate the behaviour of an electric DC motor as it turns on and off, by modelling an envelope of its frequency and volume
    use setSampleRate() before use, then call process() every sample, and call powerOn() and powerOff() to cause envelope to rise or fall
    */
    class MachineEnvelope
    {
    public:
        MachineEnvelope() {}

        //================== mutators ===================//

        /** Sets the sample rate
         * @param sr - sample rate, Hz
         */
        void setSampleRate(float sr)
        {
            sampleRate = sr;
            deltaPowerDown = 1.0f / (powerDownTimeSeconds * sampleRate);
            phase.reset(sampleRate, powerUpTimeSeconds);
        }

        /** Sets the time in seconds that it takes for the envelope to reach its max value from 0
         * @param time - power up time, seconds
         */
        void setPowerUpTime(float time)
        {
            powerUpTimeSeconds = time;
            if (sampleRate > 0.0f)
            {
                phase.reset(sampleRate, powerUpTimeSeconds);
            }
        }

        /** Sets the time in seconds that it takes for the envelope to fall from its max value to 0
         * @param time - power down time, seconds
         */
        void setPowerDownTime(float time)
        {
            powerDownTimeSeconds = time;
            deltaPowerDown = 1.0f / (powerDownTimeSeconds * sampleRate);
        }

        //=============== actions ===============//

        /** Simulates motor turning on, by causing envelope to rise to its maximum value over set time
         * @return
         */
        void powerOn()
        {
            phase.setCurrentAndTargetValue(currentEnvValue * 0.5f); // phase is doubled for powerOn curve, so have current env value here
            phase.getNextValue();
            phase.setTargetValue(1.0f);
            isOn = true;
        }

        /** Simulates motor turning off, by causing envelope to fall to 0 over set time
         * @return
         */
        void powerOff()
        {
            isOn = false;
            phase.setCurrentAndTargetValue(0.0f);
        }

        /** processes the envelope, updating the currentEnvValue and then returning this value
         * @return currentEnvValue
         */
        float process()
        {
            if (isOn)
            {
                currentEnvValue = powerUpCurveGetNextValue();
            }
            else
            {
                // linear decrease
                currentEnvValue -= deltaPowerDown;

                if (currentEnvValue <= 0)
                {
                    currentEnvValue = 0;
                }
            }

            return currentEnvValue;
        }

        //================ accessors ================//

        float getCurrentValue() { return currentEnvValue; }

        bool getIsPowerOn() { return isOn; }

    private:
        float powerUpCurveGetNextValue()
        {
            float currentPhaseVal = phase.getNextValue();

            float risingVal = 1.0f - juce::jmin(1.0f, currentPhaseVal);
            risingVal = pow(risingVal, 3.0f);

            float fallingVal = juce::jmax(1.0f, currentPhaseVal) - 1.0f;

            return 1.0f - (risingVal + fallingVal);
        }

        juce::SmoothedValue<float> phase{};
        float powerUpTimeSeconds{1.5f};   // time in seconds for envelope to rise to max value
        float powerDownTimeSeconds{1.5f}; // time in seconds for envelope to fall from max value
        float deltaPowerDown{};           // increment needed to linearly decrease volume from 1 to 0 over desired power down time
        float sampleRate{44000.0f};
        float currentEnvValue{}; // current value of the envelope
        bool isOn{false};
    };
}