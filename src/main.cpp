#include <Arduino.h>

int i = 0;

class Measurement
{
private:
    int *measurements; // Array delle misurazioni
    int length;        // Lunghezza dell'array
    int count;         // Numero di elementi attualmente presenti nell'array
    float mean;        // Media
    float std;
    float sum;               // Deviazione standard
    unsigned long timestamp; // Timestamp della prima misurazione
    bool arrayFull;

public:
    Measurement(int len);
    ~Measurement();
    void setLength(int len);
    void insertMeasurement(int value);
    void calculateMean();
    void calculateStd();
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

    if (count == length)
    {
        count = 0;
        arrayFull = true;
        calculateMean();
        calculateStd();
        sum = 0;
    }

    sum += value;
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

int Measurement::getArrayLenght()
{
    return sizeof(measurements) / sizeof(measurements[0]);
}

int Measurement::getLastMeasurement()
{
    return measurements[count];
}

Measurement measurement(860);
char serial;

void setup()
{
    Serial.begin(250000);

    while (true)
    {
        if (Serial.available() > 0)
        {
            serial = Serial.read();
            Serial.println(serial, HEX);
            if (serial == 's')
            {
                // Aggiungi qui il tuo codice da eseguire quando ricevi 's'
                break; // Esce dal ciclo while quando riceve 's'
            }
        }
    }

    Serial.println("START");

    delay(1000);

    Serial.println("CURRENT");
    Serial.println("860");

    int i = 0;
}

void loop()
{
    delayMicroseconds(1160); // Simulate the time of the conversion (860us)
    if (!measurement.isArrayFull())
    {
        // Serial.println(ads.getLastConversionResults());
        measurement.insertMeasurement(i++ + random(0, 20));
        // Serial.println(measurement.getLastMeasurement());
        Serial.write(0xCC);                                           // Start byte
        Serial.write((measurement.getLastMeasurement() >> 8) & 0xFF); // High byte
        Serial.write(measurement.getLastMeasurement() & 0xFF);        // Low byte 
    }
    else
    {
        // Serial.println("Mean: " + String(measurement.getMean()));
        // Serial.println("------------------------------------------------------------------------------");
        //  Serial.println("Std: " + String(measurement.getStd()));
        // Serial.println("Array full, resetted");
        // Serial.println("Array length: " + String(measurement.getLength()) + "\n");
        i = 0;
        measurement.setArrayFull(false);
    }
}