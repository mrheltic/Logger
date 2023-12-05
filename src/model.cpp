#include <math.h>

class Measurement
{
private:
    int *measurements;       // Array delle misurazioni
    int length;              // Lunghezza dell'array
    int count;               // Numero di elementi attualmente presenti nell'array
    float mean;              // Media
    float std;               // Deviazione standard
    unsigned long timestamp; // Timestamp della prima misurazione

public:
    Measurement(int len)
    {
        length = len;
        measurements = new int[length];
        count = 0;
        mean = 0.0;
        std = 0.0;
        timestamp = 0;
    }

    ~Measurement()
    {
        delete[] measurements;
    }

    void setLength(int len)
    {
        delete[] measurements;
        length = len;
        measurements = new int[length];
        count = 0;
        mean = 0.0;
        std = 0.0;
        timestamp = 0;
    }

    void calculateMean()
    {
        int sum = 0;
        for (int i = 0; i < length; i++)
        {
            sum += measurements[i];
        }
        mean = static_cast<float>(sum) / length;
    }

    void calculateStd()
    {
        float sum = 0.0;
        for (int i = 0; i < length; i++)
        {
            sum += pow(measurements[i] - mean, 2);
        }
        std = sqrt(sum / length);
    }

    bool isArrayFull()
    {
        return count == length;
    }

    void insertMeasurement(int value)
    {
        if (count < length)
        {
            measurements[count] = value;
            count++;
        }
    }

    void reset()
    {
        delete[] measurements;
        measurements = nullptr;
        length = 0;
        count = 0;
        mean = 0.0;
        std = 0.0;
        timestamp = 0;
    }

    float getMean()
    {
        return mean;
    }

    float getStd()
    {
        return std;
    }

    int *getMeasurements()
    {
        return measurements;
    }

    void setTimestamp(unsigned long timestamp)
    {
        this->timestamp = timestamp;
    }
};