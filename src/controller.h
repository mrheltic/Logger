// controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "view.h"
#include <Adafruit_ADS1X15.h>
#include <SD.h>

// Dichiarazione delle funzioni
boolean initializeOutputDevices();
boolean initializeInputDevices();
boolean ADCinitialize();
boolean initializeSDcard();
boolean initializeDevices();
boolean goUp();
boolean goDown();
boolean select();
void soundBuzzerSelect();
void soundBuzzerScroll();
void Logger_act();
void Output_mode_act();
void Input_mode_act();
void Info_device_act();
void Sample_set_act();
#endif // CONTROLLER_H
