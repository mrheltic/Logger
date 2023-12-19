// controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "view.h"
#include <Adafruit_ADS1X15.h>
#include <SD.h>
#include "model.h"

// Dichiazione enum
enum MODE
{
    DISPLAY_ONLY=0,
    SD_ONLY=2,
    WIFI_ONLY=1
};

enum CHANNEL
{
    VOLTAGE=0,
    CURRENT=1,
    RESISTANCE=2
};

// Dichiarazione delle variabili globali
extern MODE currentMode;
extern CHANNEL currentChannel;
extern uint16_t currentSampleRate;

// Dichiarazione delle funzioni
void initializeSerial();
boolean initializeOutputDevices();
boolean initializeInputDevices();
boolean ADCinitialize();
boolean initializeSDcard();
void logfileSDcard();
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
void setRate(uint16_t value);
void adcSetup();
void setChannel(CHANNEL channel);
#endif // CONTROLLER_H
