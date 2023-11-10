#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#include <RTClib.h>

#include "view.h"
#include "controller.h"



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

  updateMenu();
}

void loop()
{

  if (goDown())
  {
    soundBuzzer();
    menu++;
    updateMenu();
  }
  if (goUp())
  {
    soundBuzzer();
    menu--;
    updateMenu();
  }
  if (select())
  {
    soundBuzzer();
    executeAction();
    // updateMenu();
  }
  delay(40);
  Serial.print(menu);
}