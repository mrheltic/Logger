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
  boolean arrayFull;
  float sum;
  float sumsquare;
  float truemean;
  
public:
  Measurement(int len);
  ~Measurement();
  void setLength(int len);
  void insertMeasurement(int value);
  void insertMeasurementCurrent(int value);
  void calculateMean();
  void calculateStd();
  void calculateTruemean();
  void reset();
  void setTimestamp(unsigned long timestamp);
  float getMean();
  float getStd();
  float getTruemean();
  int* getMeasurements();
  boolean isArrayFull();
  int getLength();
  void setArrayFull(boolean arrayFull);
  int getArrayLenght();
  int getLastMeasurement();
};

#endif