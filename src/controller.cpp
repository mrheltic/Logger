#include <Arduino.h>
#include "view.h"
#include <Adafruit_ADS1X15.h>
#include <SD.h>

enum button
{
    LEFT,
    RIGHT,
    CENTER
};

// DECLARING VARIABLES FOR BUTTONS
#define DOWN_BUTTON 12   // GPIO D6
#define SELECT_BUTTON 13 // GPIO D7
#define UP_BUTTON 15     // GPIO D8

// DECLARING VARIABLES FOR OUTPUT DEVICES
#define BUZZER 14 // GPIO D5
// #define LED_1  //GPIO D4

Adafruit_ADS1115 ads;

File file;

int dataRateValues[] = {1, 2, 4, 8, 16, 32, 64, 128, 250, 475, 860};

boolean initializeOutputDevices()
{

    pinMode(BUZZER, OUTPUT);

    return true;
}

boolean initializeInputDevices()
{

    pinMode(UP_BUTTON, INPUT);
    pinMode(SELECT_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);

    return true;
}

boolean ADCinitialize()
{
    ads.begin();
    ads.setDataRate(250);
    ads.setGain(GAIN_TWOTHIRDS);
    return true;
}

boolean initializeSDcard()
{
    if (!SD.begin(10))
    {
        return false;
    }
    else
    {
        if (SD.exists("LOG_ads.txt"))
        {
            SD.remove("LOG_ads.txt");
        }
        return true;
    }
}

boolean initializeDevices()
{

    return initializeOutputDevices() && 
    initializeInputDevices() && 
    initializeScreen() && 
    ADCinitialize() && 
    initializeSDcard();
}

/*boolean handleButtonPress(button buttonPressed) {

  switch (buttonPressed) {

    case RIGHT:

        return digitalRead(RIGHT_BUTTON);

    case CENTER:

        return digitalRead(RIGHT_BUTTON);

    case LEFT:

        return digitalRead(LEFT_BUTTON);

    default:

        Serial.println("Unknown button pressed");
        return false;
        break;
  }
} */

boolean goUp()
{

    return digitalRead(UP_BUTTON);
}

boolean goDown()
{

    return digitalRead(DOWN_BUTTON);
}

boolean select()
{

    return digitalRead(SELECT_BUTTON);
}

void soundBuzzerScroll()
{

    tone(BUZZER, 200, 1000);

    delay(40);

    noTone(BUZZER);
}

void soundBuzzerSelect()
{

    tone(BUZZER, 1000, 300);

    delay(40);

    noTone(BUZZER);
}

void selectDataRate()
{
    int i;

    while(select()){

    }

    ads.setDataRate(dataRateValues[i])

    if(++i == sizeof(dataRateValues))
    {
        i = 0;
    }

    }
    

}
