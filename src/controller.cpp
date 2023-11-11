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

bool isExecuted = false;

Adafruit_ADS1115 ads;

File file;

int dataRateValues[] = {1, 2, 4, 8, 16, 32, 64, 128, 250, 475, 860};
int channel = 0;

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
    if (!SD.begin(15)) // is CS pin in NodemCU
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
    // Initialize only essential devices to correct work of logger
    return initializeOutputDevices() &&
           initializeInputDevices() &&
           initializeScreen() && ADCinitialize();
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

// Executive Actions
void Logger_act()
{
    // vuoto
}

void Output_mode_act()
{
    // vuoto
}

void Input_mode_act()
{
    // vuoto
}

void Info_device_act()
{
    // vuoto
}

void Sample_set_act()
{
    int Srate = int(ads.getDataRate());
    //Serial.print(Srate);
    Sample_set_grf(Srate);

    if (goDown())
    {
        int i = 0;
        soundBuzzerScroll();
        for (int j = 0; j < 11; j++)
        {
            if (dataRateValues[j] == Srate)
                i = j;
        }
        if (i > 0)
        {
            i--;
            ads.setDataRate(dataRateValues[i]);
        }
        Sample_set_selector_grf(0);
        delay(120);
    }
    if (goUp())
    {
        int i = 0;
        soundBuzzerScroll();
        for (int j = 0; j < 11; j++)
        {
            if (dataRateValues[j] == Srate)
                i = j;
        }
        Serial.println(goUp());
        if (i < 10)
        {
            i++;
            ads.setDataRate(dataRateValues[i]);
        }
        Sample_set_selector_grf(1);
        delay(120);
    }
    delay(10);
}