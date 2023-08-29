template <typename T>
class Smoother
{
public:
    Smoother(int numberOfSamples) : numberOfSamples(numberOfSamples) {}
    void setTarget(T target)
    {
        targetValue = target;
    }
    setValue(T value)
    {
        targetValue = value;
        value = value;
    }
    T getVal()
    {
        // Apply smoothing to the input value
        auto inc = (targetValue - value) / numberOfSamples;
        value += inc;
        return value;
    }

private:
    // How many samples it took to get to the target value
    int numberOfSamples = 1;
    T value = 0.0f;
    T targetValue = 0.0f;
};