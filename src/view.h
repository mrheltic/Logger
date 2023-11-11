// view.h
#ifndef VIEW_H
#define VIEW_H

#include <Adafruit_SSD1306.h>

extern int menu;
int updateMenu(int menu);
boolean initializeScreen();
void Logger_grf();
void Output_mode_grf();
void Input_mode_grf();
void Info_device_grf();
void Sample_set_grf(int sample);
void Sample_set_selector_grf(boolean arrowup);

#endif // VIEW_H