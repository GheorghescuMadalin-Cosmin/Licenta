#pragma once
#include <cmath>
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class SineWave
{
    double amplitude_;
    double phase_;
    double frequency_;
    double value_;
public:
    SineWave(double a, double p, double f)
    {
        amplitude_ = a;
        frequency_ = f;
        phase_ = p;
        uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double timeInSeconds = time / 1000.0;
        value_ = amplitude_ * sin(2.0 * M_PI * frequency_ * timeInSeconds + phase_);
    }

    double getAmplitude()
    {
        return amplitude_;
    }

    double getPhase()
    {
        return phase_;
    }

    double getFrequency()
    {
        return frequency_;
    }

    double getValue()
    {
        return value_;
    }
};
