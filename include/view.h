// view.h
#ifndef VIEW_H
#define VIEW_H

#include <Adafruit_SSD1306.h>

extern int menu;
int updateMenu(int menu);
boolean initializeScreen();
void updateContextCursor(int position);
void loggerGraphic(int mode, int channel);
void outputModeGraphic(int mode);
void inputModeGraphic(int mode);
void infoGraphic(bool WifiState, bool sdState, bool RTCState);
void sampleSetGraphic(int sample);
void sampleSetSelectorGraphic(boolean arrowup);
#endif // VIEW_H