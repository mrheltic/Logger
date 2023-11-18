#include <Arduino.h>
#include "../include/controller.h"
#include <Adafruit_ADS1X15.h>
#include <SD.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>

// DECLARING VARIABLES FOR BUTTONS
#define DOWN_BUTTON 36  // GPIO D3
#define SELECT_BUTTON 34 // GPIO D1
#define UP_BUTTON 39     // GPIO D2

// DECLARING VARIABLES FOR OUTPUT DEVICES
#define BUZZER 14 // GPIO SDD2
// #define LED_1  //GPIO D4

// DECLARING VARIABLES FOR ADS
#define ALERT_PIN 35

// DECLARING VARIABLES FOR WIFI
const char* ssid     = "Logger-Access-Point";
const char* password = "logger1234";

bool isExecuted = false;

Adafruit_ADS1115 ads;

File file;
DS1307 rtc;

int dataRateValues[] = {8, 16, 32, 64, 128, 250, 475, 860};
int channel = 0;

/*la variabile "mode" è provvisoria. Definisce la modalità con cui i dati acquisti vengono stoccati.
mode = 0 for only display, mode = 1 for wifi or mode = 2 for SD card*/
int mode = 0;

void initializeSerial(){
    // INITIALIZING SERIAL MONITOR
    Serial.begin(115200);

    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    
    Serial.println("Initializing...");


}

boolean initializeOutputDevices()
{
    Serial.println("Initializing output devices...");

    pinMode(BUZZER, OUTPUT);

    Serial.println("Output devices initialized");

    return true;
}

boolean initializeInputDevices()
{

    Serial.println("Initializing input devices...");

    pinMode(UP_BUTTON, INPUT);
    pinMode(SELECT_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);

    Serial.println("Input devices initialized");

    return true;
}

boolean ADCinitialize()
{
    Serial.println("Initializing ADC...");

    ads.begin();
    ads.setDataRate(250);
    ads.setGain(GAIN_TWOTHIRDS);

    Serial.println("ADC initialized");

    return true;
}

boolean initializeSDcard()
{
    Serial.println("Initializing SD card...");

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

    Serial.println("Initializing RTC...");

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


boolean initializeWifi(){

    Serial.println("Initializing Wifi...");

    // IP Address details
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    WebServer server(80);

    WiFi.softAP(ssid, password);
     WiFi.softAPConfig(local_ip, gateway, subnet);


    Serial.print("Connect to My access point: ");
    Serial.println(ssid);

    //server.on("/", server.send(200, "text/html", HTML));

    server.begin();
    Serial.println("HTTP server started");


}

boolean initializeDevices()
{
    Serial.println("Initializing devices...");

    initializeSerial();

    Serial.println("Initialized devices!");
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

uint16_t adsToStringRate(int value){

    return ("RATE_ADS1115_%dSPS",value);

}

void sampleSetAct()
{
    int Srate = int(ads.getDataRate());
    sampleSetGraphic(Srate);
    Serial.println(Srate);

    if (goDown())
    {
        int i = 0;
        soundBuzzerScroll();
        for (int j = 0; j < 8; j++)
        {
            if (dataRateValues[j] == Srate)
                i = j;
        }
        if (i > 0)
        {
            i--;
            ads.setDataRate(adsToStringRate(dataRateValues[i]));
        }
        sampleSetSelectorGraphic(0);
    }
    if (goUp())
    {
        int i = 0;
        soundBuzzerScroll();
        for (int j = 0; j < 8; j++)
        {
            if (dataRateValues[j] == Srate)
                i = j;
        }
        Serial.println(goUp());
        if (i <  7)
        {
            i++;
            ads.setDataRate(adsToStringRate(dataRateValues[i]));
        }
        sampleSetSelectorGraphic(1);
    }
    delay(10);
}