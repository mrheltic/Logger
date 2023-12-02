#include <Arduino.h>
#include "../include/controller.h"
#include <Adafruit_ADS1X15.h>
#include <SD.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>

// DECLARING VARIABLES FOR BUTTONS
#define DOWN_BUTTON 39  // GPIO D3
#define SELECT_BUTTON 34 // GPIO D1
#define UP_BUTTON 35     // GPIO D2

// DECLARING VARIABLES FOR OUTPUT DEVICES
#define BUZZER 33 // GPIO SDD2
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

/**
 * @brief Initializes the serial monitor.
 * 
 * This function initializes the serial monitor with a baud rate of 115200.
 * It waits for the serial port to connect before proceeding.
 * It also prints an "Initializing..." message to the serial monitor.
 */
void initializeSerial(){
    // INITIALIZING SERIAL MONITOR
    Serial.begin(115200);

    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    
    Serial.println("Initializing...");


}

/**
 * @brief Initializes the output devices.
 * 
 * This function initializes the output devices by setting the pinMode of the BUZZER pin to OUTPUT.
 * 
 * @return true if the output devices are successfully initialized, false otherwise.
 */
boolean initializeOutputDevices()
{
    Serial.println("Initializing output devices...");

    pinMode(BUZZER, OUTPUT);

    Serial.println("Output devices initialized");

    return true;
}

/**
 * @brief Initializes the input devices.
 * 
 * This function sets the pin modes for the UP_BUTTON, SELECT_BUTTON, and DOWN_BUTTON pins to INPUT.
 * 
 * @return true if the input devices are successfully initialized, false otherwise.
 */
boolean initializeInputDevices()
{

    Serial.println("Initializing input devices...");

    pinMode(UP_BUTTON, INPUT);
    pinMode(SELECT_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);

    Serial.println("Input devices initialized");

    return true;
}

/**
 * @brief Initializes the ADC.
 * 
 * This function initializes the ADC by setting the data rate and gain.
 * 
 * @return true if the ADC is successfully initialized, false otherwise.
 */
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

boolean initializeRTC(){

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

    Serial.println("Wifi initialized");
    return true;
}

/**
 * @brief Initializes the devices required for the correct operation of the logger.
 * 
 * @return true if all essential devices are successfully initialized, false otherwise.
 */
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

/**
 * @brief Function to check if the UP button is pressed.
 * 
 * @return true if the UP button is pressed, false otherwise.
 */
boolean goUp()
{
    return digitalRead(UP_BUTTON);
}

/**
 * @brief Checks if the down button is pressed.
 * 
 * @return true if the down button is pressed, false otherwise.
 */
boolean goDown()
{
    return digitalRead(DOWN_BUTTON);
}

/**
 * @brief Function to check if the select button is pressed.
 * 
 * @return true if the select button is pressed, false otherwise.
 */
boolean select()
{
    return digitalRead(SELECT_BUTTON);
}

/**
 * @brief Function to sound the buzzer in a scrolling pattern.
 * 
 * This function plays a tone on the buzzer for a duration of 1000 milliseconds (1 second),
 * followed by a delay of 40 milliseconds. Then, the buzzer is turned off using the noTone() function,
 * followed by a delay of 100 milliseconds.
 */
void soundBuzzerScroll()
{
    tone(BUZZER, 200, 1000);

    delay(40);

    noTone(BUZZER);

    delay(100);
}

/**
 * Function to sound the buzzer with a specific frequency and duration.
 */
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

/**
 * @brief Performs the information action.
 * 
 * This function checks if the subSetup flag is set or if the goUp or goDown functions return true.
 * If any of these conditions are true, it calls the infoGraphic function with the results of the initializeWifi, initializeSDcard, and initializeRTC functions as arguments.
 * It also resets the subSetup flag to 0.
 * 
 * @param subSetup A boolean flag indicating if the submenu setup is required.
 */
void infoAct(boolean subSetup)
{
    if(subSetup || goUp() || goDown()){
        infoGraphic(initializeWifi(), initializeSDcard(), initializeRTC());
        //submenu setup
        subSetup = 0;
    }
}

/**
 * @brief Converts an integer value to a string representation of the ADS1115 conversion rate.
 * 
 * @param value The ADS1115 conversion rate value.
 * @return The string representation of the ADS1115 conversion rate.
 */
uint16_t adsToStringRate(int value){

    return ("RATE_ADS1115_%dSPS",value);

}

/**
 * @brief Sets the sample act.
 * 
 * This function is responsible for setting the sample act.
 */
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