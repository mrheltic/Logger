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
  int mode;
  unsigned long timestamp; // Timestamp dell'ultima misurazione
  boolean arrayFull;
  float sum;
  
public:
  Measurement(int len);
  ~Measurement();
  void setLength(int len);
  void setMode(int mode);
  void insertMeasurement(int value);
  void insertMeasurementCurrent(int value);
  void calculateMean();
  void calculateStd();
  void reset();
  void setTimestamp(unsigned long timestamp);
  float getMean();
  float getStd();
  float getSum();
  int* getMeasurements();
  boolean isArrayFull();
  int getLength();
  void setArrayFull(boolean arrayFull);
  int getArrayLenght();
  int getLastMeasurement();
};

#endif