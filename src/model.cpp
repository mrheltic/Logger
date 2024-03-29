#include <math.h>
#include <Arduino.h>

class Measurement
{
private:
    int *measurements;       // Array delle misurazioni
    int length;              // Lunghezza dell'array
    int count;               // Numero di elementi attualmente presenti nell'array
    float mean;              // Media
    float std; 
    float sum;              // Deviazione standard
    int mode;
    unsigned long timestamp; // Timestamp della prima misurazione
    bool arrayFull;

public:
    Measurement(int len);
    ~Measurement();
    void setLength(int len);
    void setMode(int mode);
    void insertMeasurement(int value);
    void insertMeasurementCurrent(int value);
    void calculateMean();
    void calculateStd();
    float getSum();
    void reset();
    void setTimestamp(unsigned long timestamp);
    float getMean();
    float getStd();
    int *getMeasurements();
    bool isArrayFull();
    int getLength();
    void setArrayFull(bool arrayFull);
    int getArrayLenght();
    int getLastMeasurement();
};

Measurement::Measurement(int len)
{
    length = len;
    measurements = new int[length];
    count = 0;
    mean = 0.0;
    std = 0.0;
    sum = 0;
    timestamp = 0;
}

Measurement::~Measurement()
{
    delete[] measurements;
}

void Measurement::setLength(int len)
{
    delete[] measurements;
    length = len;
    measurements = new int[length];
    count = 0;
    mean = 0.0;
    std = 0.0;
    sum = 0;
    timestamp = 0;
}

// We preferred to calculate the mean in this way because we don't need to re-iterate the array when calculate the mean (so other 860 iterations in)
void Measurement::calculateMean()
{
    mean = static_cast<float>(sum) / length;
}

void Measurement::calculateStd()
{
    float sum = 0.0;
    for (int i = 0; i < length; i++)
    {
        sum += pow(measurements[i] - mean, 2);
    }
    std = sqrt(sum / length);
    sum = 0;
}

bool Measurement::isArrayFull()
{
    return arrayFull;
}

void Measurement::insertMeasurement(int value)
{
    measurements[count] = value;
    count++;

    if(count == length)
    {
        count = 0;
        arrayFull = true;
        calculateMean();
        //calculateStd();
        sum = 0;
    }

    sum += pow(value, mode);
}

void Measurement::reset()
{
    length = 0;
    count = 0;
    mean = 0.0;
    std = 0.0;
    timestamp = 0;
    arrayFull = false;
}

float Measurement::getMean()
{
    return mean;
}

float Measurement::getStd()
{
    return std;
}

float Measurement::getSum()
{
    return sum;
}

int *Measurement::getMeasurements()
{
    return this->measurements;
}

void Measurement::setTimestamp(unsigned long timestamp)
{
    this->timestamp = timestamp;
}

int Measurement::getLength()
{
    return length;
}

void Measurement::setArrayFull(bool arrayFull)
{
    this->arrayFull = arrayFull;
}

int Measurement::getArrayLenght(){
    return sizeof(measurements)/sizeof(measurements[0]);
}

int Measurement::getLastMeasurement()
{
    return measurements[count];
}

void Measurement::setMode(int mode)
{
    this->mode = mode;
}

// Print the mean of measurement returning 2 value: the 4 digits most significant and the power of 10
