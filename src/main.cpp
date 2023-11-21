#include <Arduino.h>
#include "../include/view.h"
#include "../include/controller.h"

// STATE VARIABLES
int menu = 1;
boolean stateMenu = 1;
boolean subSetup = 1;


// Start submenu loops with controller actions
void executeAction()
{
  switch (menu)
  {
  case 1:
    while (!select() && stateMenu == 0)
    {
      loggerAct();
    }
    stateMenu = 1;
    break;
  case 2:
    while (!select() && stateMenu == 0)
    {
      outputModeAct();
    }
    stateMenu = 1;
    break;
  case 3:
    while (!select() && stateMenu == 0)
    {
      inputModeAct();
    }
    stateMenu = 1;
    break;
  case 4:
    while (!select() && stateMenu == 0)
    {
      infoAct(subSetup);
    }
    stateMenu = subSetup = 1;
    break;
  case 5:
    while (!select() && stateMenu == 0)
    {
      sampleSetAct();
      //Serial.println("oi");
    }
    stateMenu = 1;
    break;
  }
  soundBuzzerSelect();
  updateMenu(menu);
}

void setup()
{

  while (!initializeDevices())
  {
    delay(1000);
  }

  updateMenu(menu);
}

void loop()
{
  if (stateMenu)
  {
    if (goDown())
    {
      soundBuzzerScroll();
      menu++;
      menu = updateMenu(menu);
    }
    if (goUp())
    {
      soundBuzzerScroll();
      menu--;
      menu = updateMenu(menu);
    }
    if (select())
    {
      stateMenu = 0;
      soundBuzzerSelect();
      executeAction();
    }
  }

  // delay(40);
  // Serial.print(menu);
}