#pragma once

#include <PhysicalModellingFan/components/audio/jr_Motor_Envelope.h>
#include <PhysicalModellingFan/components/audio/jr_SimpleFan.h>
#include <PhysicalModellingFan/components/audio/jr_PolyBLEP_Oscillators.h>

namespace jr
{
    /*
    A class that contains all of the separate mechanical sound elements such as the fan and motor,
    as well as the shared elements such as envelope and speed controls.
    This class encapsulates the shared logic used by these elements to provide a more simple API with
    which to interact with them
    */
    class Machine
    {
    public:
        Machine() {}

        void setSampleRate(float _sampleRate);

        /**
        Processes the next sample values of the system. Call getCurrentSampleLeft and getCurrentSampleRight
        to reference the new values.
        */
        void process();

        float getCurrentSampleLeft() { return currentSampleLeft; }

        float getCurrentSampleRight() { return currentSampleRight; }

        void togglePower(bool powerOn) { powerOn ? envelope.powerOn() : envelope.powerOff(); }

        //=============== Envelope Mutators ==============//

        void setPowerUpTime(float seconds) { envelope.setPowerUpTime(seconds); }
        void setPowerDownTime(float seconds) { envelope.setPowerDownTime(seconds); }

        //================= Fan Mutators =================//

        void setSpeed(float speedInHz) { fan.setSpeed(speedInHz); }
        void setFanLevel(float level) { fan.setLevel(level); }
        void setFanToneLevel(float level) { fan.setToneLevel(level); }
        void setFanNoiseLevel(float level) { fan.setNoiseLevel(level); }
        void setFanStereoWidth(float level) { fan.setPanWidth(level); }
        void setFanDoppler(bool isOn) { fan.setDopplerOn(isOn); }

    private:
        MachineEnvelope envelope{};
        FanPropeller fan{};
        float currentSampleLeft{};
        float currentSampleRight{};
    };
}