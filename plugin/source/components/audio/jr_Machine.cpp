#include <PhysicalModellingFan/components/audio/jr_Machine.h>

namespace jr
{
    void Machine::setSampleRate(float _sampleRate)
    {
        if (_sampleRate > 0)
        {
            envelope.setSampleRate(_sampleRate);
            fan.setSampleRate(_sampleRate);
            gain.reset(_sampleRate, gainSmoothingInS);
        }
    }

    void Machine::process()
    {
        envelope.process();
        fan.process(envelope.getCurrentValue());
        gain.getNextValue();

        currentSampleLeft = gain.getCurrentValue() * fan.getLeftSample() * envelope.getCurrentValue();
        currentSampleRight = gain.getCurrentValue() * fan.getRightSample() * envelope.getCurrentValue();
    }
}