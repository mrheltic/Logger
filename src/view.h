// view.h
#ifndef VIEW_H
#define VIEW_H

#include <Adafruit_SSD1306.h>

extern int menu;
void updateMenu();
void Logger_act();
void Output_mode_act();
void Input_mode_act();
void Info_device_act();
void Sample_set_act();
void executeAction();
boolean initializeScreen();

#endif // VIEW_H