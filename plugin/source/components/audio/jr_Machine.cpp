#include <PhysicalModellingFan/components/audio/jr_Machine.h>

namespace jr
{
    void Machine::setSampleRate(float _sampleRate)
    {
        if (_sampleRate > 0)
        {
            envelope.setSampleRate(_sampleRate);
            fan.setSampleRate(_sampleRate);

            fan.setMappedParams(1.0f, 4.0f, 1.0f, 1.0f, 0.5f, true); // default params hard coded for testing
        }
    }

    void Machine::process()
    {
        if (!envelope.getIsPowerOn())
        {
            // hard coded for testing until GUI is created
            envelope.powerOn();
        }

        envelope.process();
        fan.process();

        currentSampleLeft = fan.getLeftSample() * envelope.getCurrentValue();
        currentSampleRight = fan.getRightSample() * envelope.getCurrentValue();
    }
}