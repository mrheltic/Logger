// controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "view.h"
#include <Adafruit_ADS1X15.h>
#include <SD.h>

// Dichiarazione delle funzioni
void initializeSerial();
boolean initializeOutputDevices();
boolean initializeInputDevices();
boolean ADCinitialize();
boolean initializeSDcard();
boolean initializeRTC();
boolean initializeDevices();
boolean goUp();
boolean goDown();
boolean select();
void soundBuzzerSelect();
void soundBuzzerScroll();
void loggerAct();
void outputModeAct();
void inputModeAct();
void infoAct(boolean subSetup);
void sampleSetAct();
uint16_t adsToStringRate();
#endif // CONTROLLER_H
