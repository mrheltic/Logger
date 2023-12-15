#include <Arduino.h>

int i = 0;

class Measurement
{
private:
  int *measurements;       // Array delle misurazioni
  int length;              // Lunghezza dell'array
  int count;               // Numero di elementi attualmente presenti nell'array
  float mean;              // Media
  float std;               // Deviazione standard
  unsigned long timestamp; // Timestamp della prima misurazione
  float totalsum = 0;          // Somma totale delle misurazioni

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
    mean = static_cast<float>(totalsum) / length;
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
      totalsum += value;
    }
    if (isArrayFull())
    {
      calculateMean();
      calculateStd();
      totalsum = 0;
    }
  }

  void clearMeasurements()
  {
    count = 0;
    mean = 0.0;
    std = 0.0;
    memset(measurements, 0, sizeof(measurements));
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

Measurement m(860);
unsigned long time_now;
unsigned long time_old;

void setup()
{
  Serial.begin(9600);
}

void loop()
{

  m.insertMeasurement(random(0, 1024));
  i++;

  if (m.isArrayFull())
  {
    //Serial.println("Mean: " + String(m.getMean()));
    //Serial.println("Std: " + String(m.getStd()));
    Serial.write((const uint8_t*)m.getMeasurements(), 860);

    i = 0;
    m.clearMeasurements();
    Serial.println("Cleared");

    time_now = millis();
    Serial.print("ci ho messo: ");
    Serial.println(time_now - time_old + 140);
    time_old = time_now;
  }
}