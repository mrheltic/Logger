#include <Arduino.h>
#include "../include/libraries/Rtc-2.4.2/src/RtcDS1302.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Hello World!");
}

void loop() {
  Serial.println("Hello World!");
  delay(1000);
}