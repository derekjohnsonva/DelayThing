#pragma once

template <typename T>
class Smoother
{
public:
    Smoother(T decayValue) : decay(decayValue) {}
    void setTarget(T target)
    {
        targetValue = target;
    }
    void setDecay(T delayHalfLife, float sampleRate)
    {
        this->decay = std::exp2(1 / (delayHalfLife * sampleRate));
    }

    T getVal()
    {
        // check to see if value and target are roughly the same
        if (std::abs(value - targetValue) < error_tolerance)
        {
            return value;
        }
        // Apply smoothing to the input value
        auto inc = (targetValue - value) * decay;
        value += inc;
        return value;
    }

private:
    // How many samples it took to get to the target value
    T decay = 1;
    float error_tolerance = 0.0001f;
    T value = 0.0f;
    T targetValue = 0.0f;
};
