// view.h
#ifndef VIEW_H
#define VIEW_H

#include <Adafruit_SSD1306.h>
#include "controller.h"

extern int menu;
int updateMenu(int menu);
boolean initializeScreen();
void updateContextCursor(int position);
void errorMessageGraphic(int currentMode);
void waitSerialGraphic();
void loggerGraphic(int mode, int channel, String currentTime, float measure);
void outputModeGraphic(int mode);
void inputModeGraphic(int channel);
void infoGraphic(bool WifiState, bool sdState, bool RTCState);
void sampleSetGraphic(int sample);
void sampleSetSelectorGraphic(boolean arrowup);
#endif // VIEW_H