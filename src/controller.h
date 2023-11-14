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
boolean inizializeRTC();
boolean initializeDevices();
boolean goUp();
boolean goDown();
boolean select();
void soundBuzzerSelect();
void soundBuzzerScroll();
void loggerAct();
void outputModeAct();
void inputModeAct();
void infoAct();
void sampleSetAct();
#endif // CONTROLLER_H
