#include <Arduino.h>
#include "view.h"
#include <Adafruit_ADS1X15.h>
#include <SD.h>
#include <RTClib.h>

enum button
{
    LEFT,
    RIGHT,
    CENTER
};

// DECLARING VARIABLES FOR BUTTONS
#define DOWN_BUTTON 12  // GPIO D3
#define SELECT_BUTTON 13 // GPIO D1
#define UP_BUTTON 15     // GPIO D2

// DECLARING VARIABLES FOR OUTPUT DEVICES
#define BUZZER 14 // GPIO SDD2
// #define LED_1  //GPIO D4

bool isExecuted = false;

Adafruit_ADS1115 ads;

File file;
DS1307 rtc;

int dataRateValues[] = {1, 2, 4, 8, 16, 32, 64, 128, 250, 475, 860};
int channel = 0;

/*la variabile "mode" è provvisoria. Definisce la modalità con cui i dati acquisti vengono stoccati.
mode = 0 for only display, mode = 1 for wifi or mode = 2 for SD card*/
int mode = 0;

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

boolean inizializeRTC(){
    if (!rtc.begin())
    {
        Serial.println("Error");
        return false;
    }
    else
    {
        if (!rtc.isrunning())
        {
            Serial.println("RTC is NOT running!");
            rtc.adjust(DateTime(__DATE__, __TIME__));
        }
        return true;
    }
}

boolean initializeDevices()
{
    // Initialize only essential devices to correct work of logger
    return initializeOutputDevices() &&
           initializeInputDevices() &&
           initializeScreen() && ADCinitialize() && inizializeRTC();
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

    delay(100);
}

void soundBuzzerSelect()
{
    tone(BUZZER, 1000, 300);

    delay(40);

    noTone(BUZZER);

    delay(250);
}

// Executive Actions
void loggerAct()
{
    loggerGraphic(mode, channel);
}

void outputModeAct()
{
    /*Attention, before selecting the data storage method, it is necessary to verify and Initialization of devices*/
    outputModeGraphic(mode);
    if (goDown())
    {
        soundBuzzerScroll();
        if (mode == 0)
            mode = 1;
        else if (mode == 2)
            mode = 0;
    }
    if (goUp())
    {
        soundBuzzerScroll();
        if (mode == 0)
            mode = 2;
        else if (mode == 1)
            mode = 0;
    }
    delay(10);
}

void inputModeAct()
{
    inputModeGraphic(channel);
    if (goDown())
    {
        soundBuzzerScroll();
        if (channel == 0)
            channel = 1;
        else if (channel == 2)
            channel = 0;
    }
    if (goUp())
    {
        soundBuzzerScroll();
        if (channel == 0)
            channel = 2;
        else if (channel == 1)
            channel = 0;
    }
    delay(10);
}

void infoAct()
{
    // vuoto
}

void sampleSetAct()
{
    int Srate = int(ads.getDataRate());
    // Serial.print(Srate);
    sampleSetGraphic(Srate);

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
        sampleSetSelectorGraphic(0);
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
        sampleSetSelectorGraphic(1);
    }
    delay(10);
}