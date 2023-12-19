#include <Arduino.h>
#include "../include/controller.h"
#include "../include/model.h"
#include "../include/view.h"
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_BusIO_Register.h>
#include <Ds1302.h>

// DECLARING VARIABLES FOR BUTTONS
#define DOWN_BUTTON 39
#define SELECT_BUTTON 34
#define UP_BUTTON 35

// DECLARING VARIABLES FOR RTC
#define PIN_ENA 14
#define PIN_CLK 26
#define PIN_DAT 27

// DECLARING VARIABLES FOR RTC
const static char *WeekDays[] =
    {
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday"};

// DECLARING VARIABLES FOR OUTPUT DEVICES
#define BUZZER 23

// DECLARING VARIABLES FOR ADS
#define ALERT_PIN 32

int dataRateValues[] = {8, 16, 32, 64, 128, 250, 475, 860};

// DECLARING VARIABLES FOR MODE AND CHANNEL
MODE currentMode = DISPLAY_ONLY;
CHANNEL currentChannel = VOLTAGE;

uint16_t currentSampleRate = 860;

unsigned long time_now = 0;
unsigned long time_old = 0;
int count = 0;

Measurement measurement(8);

// DECLARING VARIABLES FOR ADC
#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

/**
 * @brief Indicates whether new data is available.
 */
volatile bool new_data = false;
/**
 * @brief Interrupt service routine for handling new data ready event.
 *
 * This function is called when new data is ready to be processed.
 * It sets the `new_data` flag to true.
 */
void IRAM_ATTR NewDataReadyISR()
{
    new_data = true;
}

// DECLARING VARIABLES FOR WIFI
const char *ssid = "Logger-Access-Point";
const char *password = "logger1234";

bool isExecuted = false;

File file;

Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);

Adafruit_ADS1115 ads;

/**
 * @brief Initializes the serial monitor.
 *
 * This function initializes the serial monitor with a baud rate of 115200.
 * It waits for the serial port to connect before proceeding.
 * It also prints an "Initializing..." message to the serial monitor.
 */
void initializeSerial()
{
    // INITIALIZING SERIAL MONITOR
    Serial.begin(250000);

    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    // Print contributors
    Serial.println("-------------------------------");
    Serial.print("Logger\n");
    Serial.println("-------------------------------");
    Serial.println("Contributors:");
    Serial.println("- Vincenzo Pio Florio");
    Serial.println("- Francesco Stasi");
    Serial.println("- Davide Tonti");
    Serial.println("-------------------------------\n");
    Serial.println("Initializing...\n");
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

    Serial.println("Output devices initialized\n");

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

    Serial.println("Input devices initialized\n");

    return true;
}

boolean initializeSDcard()
{
    Serial.println("Initializing SD card...\n");

    if (!SD.begin(5)) // is CS pin in ESP32!!!
        return false;
    else
    {
        logfileSDcard();
        return true;
    }
}

void logfileSDcard()
{
    File creditsFile;
    File dataStorage;

    
    if (SD.exists("/dataStorage.txt")) // if the file exists it'll be removed
    {
        SD.remove("/dataStorage.txt");
    }

    Serial.println("Creating dataStorage.txt..."); // create and open the file ready to be written
    creditsFile = SD.open("/creditsFile.txt", FILE_WRITE);

    if (creditsFile)
    { // check if the file is actually opened

        // write a string to the card, once the content is written, close the file.
        creditsFile.println("-------------------------------");
        creditsFile.print("Logger\n");
        creditsFile.println("-------------------------------");
        creditsFile.println("Contributors:");
        creditsFile.println("- Vincenzo Pio Florio");
        creditsFile.println("- Francesco Stasi");
        creditsFile.println("- Davide Tonti");
        creditsFile.println("-------------------------------\n");
        creditsFile.println("Initializing...\n");
        creditsFile.close();
        dataStorage = SD.open("/dataStorage.txt", FILE_WRITE);
        dataStorage.println("test");
        dataStorage.close();

        // TODO the function to write the array of measurements by 'file.write(array)' or 'file.write(array,length)'
    }
    else
    {
        Serial.println("Error opening the file on SDcard");
    }
}

boolean initializeRTC()
{
    rtc.init();
    return true;
}

boolean initializeWifi()
{

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

    // server.on("/", server.send(200, "text/html", HTML));

    server.begin();
    Serial.println("HTTP server started");

    Serial.println("Wifi initialized\n");
    return true;
}

boolean initializeADC() // TODO finish function
{
    Serial.println("Initializing ADC...");

    Serial.println("ADC initialized");

    ads.setGain(GAIN_TWOTHIRDS); // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)

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

    Serial.println("Initialized devices!\n");
    // Initialize only essential devices to correct work of logger
    return initializeOutputDevices() &&
           initializeInputDevices() &&
           initializeScreen() && initializeADC() && initializeRTC() && initializeSDcard();
}

/**
 * @brief Function to check if the device should go up.
 *
 * This function checks if the UP_BUTTON is pressed or if the character 'u' is received from Serial.
 *
 * @return true if the device should go up, false otherwise.
 */
boolean goUp()
{
    if (Serial.available() > 0)
    {
        char ch = Serial.read();
        return (digitalRead(UP_BUTTON) || ch == 'u');
    }
    return digitalRead(UP_BUTTON);
}

/**
 * @brief Function to check if the device should go down.
 *
 * This function checks if there is data available on the Serial port. If there is, it reads the data and checks if it is equal to 'd'.
 * If the DOWN_BUTTON is pressed or the data is equal to 'd', it returns true. Otherwise, it returns false.
 *
 * @return true if the device should go down, false otherwise.
 */
boolean goDown()
{
    if (Serial.available() > 0)
    {
        char ch = Serial.read();
        return (digitalRead(DOWN_BUTTON) || ch == 'd');
    }
    return digitalRead(DOWN_BUTTON);
}

/**
 * @brief Checks if the select button is pressed or if there is data available on the Serial port.
 *
 * @return true if the select button is pressed or if there is data available on the Serial port, false otherwise.
 */
boolean select()
{
    if (Serial.available() > 0)
    {
        char ch = Serial.read();
        return (digitalRead(SELECT_BUTTON) || ch == 's');
    }
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

/**
 * @brief Returns the current time stamp as a String.
 * 
 * @return String The current time stamp in the format "HH:MM:SS".
 */
String getTimeStamp()
{

    Ds1302::DateTime now;
    rtc.getDateTime(&now);

    String currentTime = "";
    if (now.hour < 10)
        currentTime = currentTime + "0";
    currentTime = currentTime + now.hour + ":"; // 00-23
    if (now.minute < 10)
        currentTime = currentTime + "0";
    currentTime = currentTime + now.minute + ":"; // 00-59
    if (now.second < 10)
        currentTime = currentTime + "0";
    currentTime = currentTime + now.second; // 00-59

    return currentTime;
}


/**
 * @brief This function handles the output mode activation.
 */
void outputModeAct()
{
    /*Attention, before selecting the data storage method, it is necessary to verify and Initialization of devices*/
    outputModeGraphic(currentMode);
    switch (currentMode)
    {
    case SD_ONLY:

        if (goDown())
        {
            soundBuzzerScroll();
            currentMode = DISPLAY_ONLY;
            Serial.println("Mode selected: DISPLAY_ONLY\n");
        }
        if (goUp())
        {
            soundBuzzerScroll();

            currentMode = WIFI_ONLY;
            Serial.println("Mode selected: WIFI_ONLY\n");
        }
        break;
    case DISPLAY_ONLY:

        if (goDown())
        {
            soundBuzzerScroll();
            currentMode = WIFI_ONLY;
            Serial.println("Mode selected: WIFI_ONLY\n");
        }
        if (goUp()) // Check on sd card
        {
            soundBuzzerScroll();
            currentMode = SD_ONLY;
            Serial.println("Mode selected: SD_ONLY\n");
        }
        break;

    case WIFI_ONLY:

        if (goDown())
        {
            soundBuzzerScroll();
            currentMode = SD_ONLY;
            Serial.println("Mode selected: SD_ONLY\n");
        }
        if (goUp())
        {
            soundBuzzerScroll();
            currentMode = DISPLAY_ONLY;
            Serial.println("Mode selected: DISPLAY_ONLY\n");
        }
        break;

    default:
        Serial.println("\n\n\n\n-----------------------------");
        Serial.println("Error selecting channel\n");
        Serial.println("\n\n\n\n-----------------------------");
        break;
    }

    delay(10);
}


/**
 * @brief Performs the action for the input mode.
 * 
 * This function is responsible for handling the input mode and performing the necessary actions.
 */
void inputModeAct()
{
    inputModeGraphic(currentChannel);
    switch (currentChannel)
    {
    case VOLTAGE:

        if (goDown())
        {
            soundBuzzerScroll();
            currentChannel = CURRENT;
            Serial.println("Input selected: CURRENT\n");
        }
        if (goUp())
        {
            soundBuzzerScroll();
            currentChannel = RESISTANCE;
            Serial.println("Input selected: RESISTANCE\n");
        }
        break;

    case CURRENT:

        if (goDown())
        {
            soundBuzzerScroll();
            currentChannel = RESISTANCE;
            Serial.println("Input selected: RESISTANCE\n");
        }
        if (goUp())
        {
            soundBuzzerScroll();
            currentChannel = VOLTAGE;
            Serial.println("Input selected: VOLTAGE\n");
        }
        break;

    case RESISTANCE:

        if (goDown())
        {
            soundBuzzerScroll();
            currentChannel = VOLTAGE;
            Serial.println("Input selected: VOLTAGE\n");
        }
        if (goUp())
        {
            soundBuzzerScroll();
            currentChannel = CURRENT;
            Serial.println("Input selected: CURRENT\n");
        }
        break;

    default:
        Serial.println("\n\n\n\n-----------------------------");
        Serial.println("Error selecting channel");
        Serial.println("\n\n\n\n-----------------------------");
        break;
    }
    delay(10);
}


/**
 * @brief Sets the channel for the controller.
 */
void setChannel()
{
    if (currentChannel == VOLTAGE)
    {
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, true);
        Serial.println("Reading channel A0\n");
    }

    else if (currentChannel == CURRENT)
    {
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_DIFF_2_3, true);
        Serial.println("Reading channel A2-A3\n");
    }

    else if (currentChannel == RESISTANCE)
    {
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, true);
        Serial.println("Reading channel A1\n");
    }

    else
    {
        Serial.println("\n\n\n\n-----------------------------");
        Serial.println("Error selecting channel");
        Serial.println("\n\n\n\n-----------------------------");
    }
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
    if (subSetup || goUp() || goDown())
    {
        // infoGraphic(initializeWifi(), initializeSDcard(), initializeRTC()); //TODO fix methods
        //  submenu setup
        subSetup = 0;
    }
}


/**
 * @brief Sets the rate value.
 * 
 * This function sets the rate value to the specified value.
 * 
 * @param value The rate value to be set.
 */
void setRate(uint16_t value)
{

    switch (value)
    {
    case 860:
        ads.setDataRate(RATE_ADS1115_860SPS);
        break;
    case 475:
        ads.setDataRate(RATE_ADS1115_475SPS);
        break;
    case 250:
        ads.setDataRate(RATE_ADS1115_250SPS);
        break;
    case 128:
        ads.setDataRate(RATE_ADS1115_128SPS);
        break;
    case 64:
        ads.setDataRate(RATE_ADS1115_64SPS);
        break;
    case 32:
        ads.setDataRate(RATE_ADS1115_32SPS);
        break;
    case 16:
        ads.setDataRate(RATE_ADS1115_16SPS);
        break;
    case 8:
        ads.setDataRate(RATE_ADS1115_8SPS);
        break;
    default:
        Serial.println("ErrorSetDataRate");
        break;
    }
}


/**
 * @brief Sets the sample act.
 */
void sampleSetAct()
{
    sampleSetGraphic(currentSampleRate);

    if (goDown())
    {
        int i = 0;
        soundBuzzerScroll();
        for (int j = 0; j < 8; j++)
        {
            if (dataRateValues[j] == currentSampleRate)
                i = j;
        }
        if (i > 0)
        {
            i--;
            currentSampleRate = dataRateValues[i];
        }
        sampleSetSelectorGraphic(0);
        Serial.println("Selected sample rate: " + String(currentSampleRate) + "\n");
    }
    if (goUp())
    {
        int i = 0;
        soundBuzzerScroll();
        for (int j = 0; j < 8; j++)
        {
            if (dataRateValues[j] == currentSampleRate)
                i = j;
        }
        Serial.println(goUp());
        if (i < 7)
        {
            i++;
            currentSampleRate = dataRateValues[i];
        }
        sampleSetSelectorGraphic(1);
        Serial.println("Selected sample rate: " + String(currentSampleRate) + "\n");
    }
    delay(10);
}

/**
 * @brief Sets up the ADC (Analog-to-Digital Converter).
 *
 * This function initializes the ADC module and configures its settings.
 * It prepares the ADC for reading analog values from sensors or other sources.
 */
void adcSetup()
// TODO pass the conversion value to labview<
{
    Serial.println("\n\n\n\n-----------------------------");
    Serial.println("ENTERED IN ADC SETUP\n\n\n\n");

    // We get a falling edge every time a new sample is ready.
    attachInterrupt(digitalPinToInterrupt(ALERT_PIN), NewDataReadyISR, FALLING);

    // Debugging line to notify that the interrupt is attached.
    Serial.println("Interrupt attached (falling edge for new data ready)))");

    // Set the sample rate.
    setRate(currentSampleRate);
    Serial.println("Sample rate setting done!\n");

    // Initialize the ADS1115 module, if not loops forever.
    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1)
            ;
    }

    // Debugging line to print the sample rate.
    Serial.println("Sample rate: " + String(currentSampleRate) + "SPS\n");

    // Set the channel.
    setChannel();
    Serial.println("Channel setting done!\n");

    // Set the array length, equal to the sample rate.
    measurement.setLength(currentSampleRate);

    // Debugging line to print the array length, equal to the sample rate.
    Serial.println("Array length: " + String(measurement.getLength()) + "\n");
    
    // Debugging line to print the data rate.
    Serial.println("ADS setRate: " + String(ads.getDataRate()) + "\n");

    Serial.println("EXITED FROM ADC SETUP");
    Serial.println("\n\n\n\n-----------------------------");

    loggerGraphic(currentMode, currentChannel, getTimeStamp());
}

// Executive Actions
void loggerAct()
{

    switch (currentMode)
    {
    case SD_ONLY:
        Serial.println("Not implemented yet");
        break;

    case WIFI_ONLY:
        Serial.println("Not implemented yet");
        break;

    case DISPLAY_ONLY:

        if (!new_data)
        {
            // Serial.println("No new data ready!");
            return;
        }

        // Serial.println("New data ready!");

        if (!measurement.isArrayFull())
        {
            // Serial.println(ads.getLastConversionResults());
            measurement.insertMeasurement(ads.getLastConversionResults());
        }
        else
        {
            // Serial.write(measurement.getMeasurements(), sizeof(measurement.getMeasurements()));
            Serial.println("Mean: " + String(measurement.getMean()));
            Serial.println("------------------------------------------------------------------------------");
            // Serial.println("Std: " + String(measurement.getStd()));
            Serial.println("Array full, resetted");
            Serial.println("Array length: " + String(measurement.getLength()) + "\n");
            measurement.setArrayFull(false);
            loggerGraphic(currentMode, currentChannel, getTimeStamp());
        }

        new_data = false;

        break;

    case default:
        Serial.println("\n\n\n\n-----------------------------");
        Serial.println("Error selecting output\n");
        Serial.println("\n\n\n\n-----------------------------");
        break;
    }
}
