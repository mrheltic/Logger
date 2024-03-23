/***************************************************
 *  Logger
 *
 * Contributors:
 * - Vincenzo Pio Florio
 * - Francesco Stasi
 * - Davide Tonti
 *
 ***************************************************/

#include <Arduino.h>
#include "../include/view.h"
#include "../include/controller.h"

// STATE VARIABLES
int menu = 1;
boolean stateMenu = 1;
boolean subSetup = 1;

// Start submenu loops with controller actions
/**
 * Executes the selected action based on the value of the 'menu' variable.
 * The action is determined by the value of 'menu' and includes options like loggerAct, outputModeAct, inputModeAct, infoAct, and sampleSetAct.
 * After executing the action, it updates the menu and sounds the buzzer.
 */
void executeAction()
{
  switch (menu)
  {
  case 1:
    adcSetup();
    if (preliminaryControl())
    {
      switch (currentMode)
      {
      case 0:
        while (!select() && stateMenu == 0)
        {
          loggerActDisplay();
        }
        break;

      case 1:
        while (!select() && stateMenu == 0)
        {
          loggerActSerial();
        }
        break;

      case 2:
        while (!select() && stateMenu == 0)
        {
          loggerActSD();
        }
        break;
      }
    }
    else
    {
      if (currentMode == SD_ONLY)
        ESP.restart();
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
    }
    stateMenu = 1;
    break;
  }
  soundBuzzer(selectFrequency, selectDuration);
  updateMenu(menu);
}

/**
 * @brief Initializes the devices and updates the menu.
 */
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
      soundBuzzer(scrollFrequency, scrollDuration);
      menu++;
      menu = updateMenu(menu);
    }
    if (goUp())
    {
      soundBuzzer(scrollFrequency, scrollDuration);
      menu--;
      menu = updateMenu(menu);
    }
    if (select())
    {
      stateMenu = 0;
      soundBuzzer(selectFrequency, selectFrequency);
      executeAction();
    }
  }

  // delay(40);
  // Serial.print(menu);
}