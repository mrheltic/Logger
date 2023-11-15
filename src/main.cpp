#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>


// This script is adapted from the continuous example given by Adafruit's ADS1X15 library.

Adafruit_ADS1115 myADS;
unsigned long t0;
int16_t v;
String str;

// The ADC module sends a interrupt signal to the Arduino when a conversion is completed.
// This way, we will read the ADC only if it is ready.
// We need to connect ALERT/RDY pin on the ADC1115 to pin 3 on the Arduino.
const int intPin = 3;
volatile bool new_data = false;
void newDataReady() {
    new_data = true;
}

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);
  Serial.begin(115200);

  // The convertion is ready on the falling edge of a pulse at the ALERT/RDY pin.
  attachInterrupt(digitalPinToInterrupt(intPin), newDataReady, FALLING);

  myADS.begin();
  myADS.setGain(GAIN_FOUR);
  myADS.setDataRate(RATE_ADS1115_860SPS);
  myADS.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, true);
  // We should set "continuous" to true to reach optimal speed.
  
  t0 = micros();
}

void loop() {
  if(!new_data) return;
  // Don't call the ADC until we receive a convertion complete signal.

  v = myADS.getLastConversionResults();
  str = micros() - t0;
  str += " ";
  str += v;
  Serial.println(str);

  new_data = false;
}