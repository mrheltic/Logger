#ifndef MODEL_H
#define MODEL_H

#include <math.h>
#include <Arduino.h>

class Measurement {
private:
  int* measurements; // Array delle misurazioni
  int length; // Lunghezza dell'array
  int count; // Numero di elementi attualmente presenti nell'array
  float mean; // Media
  float std; // Deviazione standard
  unsigned long timestamp; // Timestamp dell'ultima misurazione
  
public:
  Measurement(int len);
  ~Measurement();
  void setLength(int len);
  void insertMeasurement(uint16_t value);
  void calculateMean();
  void calculateStd();
  void reset();
  void setTimestamp(unsigned long timestamp);
  float getMean();
  float getStd();
  uint16_t* getMeasurements();
  boolean isArrayFull();
};

#endif