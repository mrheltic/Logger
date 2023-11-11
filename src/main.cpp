#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#include <RTClib.h>

#include "view.h"
#include "controller.h"

//STATE VARIABLES
int menu = 1;
boolean stateMenu = 1;

//Start submenu loops with controller actions
void executeAction()
{
  switch (menu)
  {
  case 1:
    while (!select() && stateMenu == 0)
    {
      Logger_act();
    }
    stateMenu = 1;
    delay(250); 
    break;
  case 2:
  while (!select() && stateMenu == 0)
    {
      Output_mode_act();
    }
    stateMenu = 1;
    delay(250);
    break;
  case 3:
  while (!select() && stateMenu == 0)
    {
      Input_mode_act();
    }
    stateMenu = 1;
    delay(250); 
    break;
  case 4:
  while (!select() && stateMenu == 0)
    {
      Info_device_act();
    }
    stateMenu = 1;
    delay(250);
    break;
  case 5:
  while (!select() && stateMenu == 0)
    {
      Sample_set_act();
    }
    stateMenu = 1;
    delay(250);   
    break;
  }
}

void setup()
{
  // INITIALIZING SERIAL MONITOR
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Initializing...");

  while(!initializeDevices()){
    Serial.println("Initialization failed");
    delay(1000);
  }
  Serial.println("Devices initialized");

  updateMenu(menu);
}

void loop()
{
  if(stateMenu){
    if (goDown())
  {
    soundBuzzerScroll();
    menu++;
    updateMenu(menu);
  }
  if (goUp())
  {
    soundBuzzerScroll();
    menu--;
    updateMenu(menu);
  }
  if (select())
  {
    soundBuzzerSelect();
    executeAction();
  }
  }
  
  //delay(40);
  //Serial.print(menu);
}