#include <Arduino.h>
#include "../include/controller.h"
#include "../include/model.h"
#include "../include/view.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_BusIO_Register.h>
#include <Ds1302.h>

// DECLARING VARIABLES FOR BUTTONS
#define DOWN_BUTTON 35
#define SELECT_BUTTON 34
#define UP_BUTTON 39

// DECLARING VARIABLES FOR RTC
#define PIN_ENA 14
#define PIN_CLK 26
#define PIN_DAT 27

// DECLARING VARIABLES FOR SPI
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

int16_t adcValue;

// Variables current measurement
const float FACTOR_I = 30;             // 30A/1V from teh current transformer
const float multiplier_I = 0.00003125; // for current measurement and gain four (1.024V / 2^16 * 2)
// const float multiplier_I = 0.000015625; // for current measurement and gain eight (0.512 / 2^16 * 2)

// Variables voltage measurement
const float FACTOR_V = 4.334335237;   // FACTOR_V = (R1 + R2) / R2    R1 resistor beetween Vin and A0 [ohm] and R2 resistor beetween A0 and GND [ohm]
const float multiplier_V = 0.0001875; // for voltage measurement and gain twothirds (6.144V / 2^16 * 2)
//const float multiplier_V = 0.000125;

// Variables resistance measurement
const float FACTOR_R = 999;          // FACTOR_R = R3 resistor beetween A1 and GND [ohm]
const float multiplier_R = 0.000125; // for voltage measurement and unit gain (4.096 / 2^16 * 2)

float K_value;
float O_value;

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
#define BUZZER 33
int scrollFrequency = 200;
int scrollDuration = 200;
int selectFrequency = 200;
int selectDuration = 200;

// DECLARING VARIABLES FOR ADS
#define ALERT_PIN 32

int dataRateValues[] = {8, 16, 32, 64, 128, 250, 475, 860};

// DECLARING VARIABLES FOR MODE AND CHANNEL DEFAULT CONTIONS
MODE currentMode = DISPLAY_ONLY;
CHANNEL currentChannel = VOLTAGE;
String currentChannelString = "Voltage"; // Used to communicate the current channel to the user through the serial

int currentSampleRate = 860;

// DECLARING VARIABLES FOR EMPHIRICALLY EVALUATE PERFORMANCES
unsigned long serialWaitingTime = 0;
unsigned long time_now = 0;
String currentTime;

// DECLARING TIMEOUT RESPONSE SERIAL
#define TIMEOUT 10000

// DECLARING THE OBJECT OF MEASUREMENTS
Measurement measurement(8);

// DECLARING VARIABLES FOR SD CARD
File file;

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

// DECLARING RTC
Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);

// DECLARING ADC
Adafruit_ADS1115 ads;

const char *creditString = "-------------------------------\nLogger\n-------------------------------\nContributors:\n- Vincenzo Pio Florio\n- Francesco Stasi\n- Davide Tonti\n-------------------------------\n";

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
    Serial.println(creditString);
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

    SPI.begin(SCK, MISO, MOSI, CS);

    if (!SD.begin(CS, SPI))
    {
        Serial.println("Card Mount Failed");
        return false;
    }
    else
    {
        logfileSDcard();
    }
    return true;
}

void logfileSDcard()
{

    if (SD.exists("/dataStorage.ds32")) // if the file exists it'll be removed
    {
        SD.remove("/dataStorage.ds32");
    }

    Serial.println("Creating dataStorage.ds32..."); // create and open the file ready to be written
    writeFile(SD, "/dataStorage.ds32", "");
    writeFile(SD, "/creditsFile.txt", creditString);
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    // Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        // Serial.println("File written");
    }
    else
    {
        // Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    File file = fs.open(path, FILE_APPEND);
    /*
    if (!file)
    {
        // Serial.println("Failed to open file for appending");
        return;
    }
    if ()
    {
        // Serial.println("Message appended");
    }
    else
    {
        // Serial.println("Append failed");
    }
    */
    file.print(message);
    file.close();
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

    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (10)
            ;
    }

    Serial.println("ADC initialized");

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
           initializeScreen() && initializeADC() && initializeRTC();
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
void soundBuzzer(int frequency, int duration)
{
    int period = 1000000 / frequency;
    int halfPeriod = period / 2;
    int cycles = frequency * duration / 1000;
    for (int i = 0; i < cycles; i++)
    {
        digitalWrite(BUZZER, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(BUZZER, LOW);
        delayMicroseconds(halfPeriod);
    }
}

/**
 * @brief Get the current timestamp as a string.
 *
 * @return String The current timestamp as a string.
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

String getDateStamp()
{
    Ds1302::DateTime now;
    rtc.getDateTime(&now);


    String currentDate = "";

    if (now.month < 10)
        currentDate = currentDate + "0";
    currentDate = currentDate + now.month + "/"; // 01-12
    if (now.day < 10)
        currentDate = currentDate + "0";
    currentDate = currentDate + now.day; // 01-3
    currentDate = currentDate + "/" + now.year; // 2021

    return currentDate;
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
            soundBuzzer(scrollFrequency, scrollDuration);
            currentMode = DISPLAY_ONLY;
            Serial.println("Mode selected: DISPLAY_ONLY\n");
        }
        if (goUp())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentMode = SERIAL_ONLY;
            Serial.println("Mode selected: SERIAL_ONLY\n");
        }
        break;
    case DISPLAY_ONLY:

        if (goDown())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentMode = SERIAL_ONLY;
            Serial.println("Mode selected: SERIAL_ONLY\n");
        }
        if (goUp()) // Check on sd card
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentMode = SD_ONLY;
            Serial.println("Mode selected: SD_ONLY\n");
        }
        break;

    case SERIAL_ONLY:

        if (goDown())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentMode = SD_ONLY;
            Serial.println("Mode selected: SD_ONLY\n");
        }
        if (goUp())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
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
 * This function is responsible for handling the input mode action.
 * It performs the necessary operations based on the current input mode.
 *
 * @return void
 */
void inputModeAct()
{
    inputModeGraphic(currentChannel);
    switch (currentChannel)
    {
    case VOLTAGE:

        if (goDown())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentChannel = CURRENT;
            Serial.println("Input selected: CURRENT\n");
        }
        if (goUp())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentChannel = RESISTANCE;
            Serial.println("Input selected: RESISTANCE\n");
        }
        break;

    case CURRENT:

        if (goDown())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentChannel = RESISTANCE;
            Serial.println("Input selected: RESISTANCE\n");
        }
        if (goUp())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentChannel = VOLTAGE;
            Serial.println("Input selected: VOLTAGE\n");
        }
        break;

    case RESISTANCE:

        if (goDown())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
            currentChannel = VOLTAGE;
            Serial.println("Input selected: VOLTAGE\n");
        }
        if (goUp())
        {
            soundBuzzer(scrollFrequency, scrollDuration);
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
 * Sets the channel for ADC readings.
 *
 * The function starts ADC reading based on the current channel value.
 * If the current channel is VOLTAGE, it starts ADC reading for single-ended channel 0.
 * If the current channel is CURRENT, it starts ADC reading for differential channel 2-3.
 * If the current channel is RESISTANCE, it starts ADC reading for single-ended channel 1.
 * If the current channel is none of the above, it prints an error message.
 *
 * @return void
 */
void setChannel()
{
    if (currentChannel == VOLTAGE)
    {
        ads.setGain(GAIN_TWOTHIRDS); // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, true);
        measurement.setMode(1);
        Serial.println("Reading channel A0\n");
        currentChannelString = "Voltage";
    }

    else if (currentChannel == CURRENT)
    {
        ads.setGain(GAIN_FOUR);
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_DIFF_2_3, true);
        measurement.setMode(2);
        Serial.println("Reading channel A2-A3\n");
        currentChannelString = "Current";
    }

    else if (currentChannel == RESISTANCE)
    {
        ads.setGain(GAIN_ONE);
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, true);
        measurement.setMode(1);
        Serial.println("Reading channel A1\n");
        currentChannelString = "Resistance";
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
    infoGraphic(getTimeStamp(), getDateStamp());
}

/**
 * @brief Sets the rate value.
 *
 * This function sets the rate value to the specified value.
 *
 * @param value The rate value to be set.
 */

void setRate(int value)
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
 * Function to perform the sample set action.
 * This function updates the sample rate based on user input and displays the selected sample rate.
 */
void sampleSetAct()
{
    sampleSetGraphic(currentSampleRate);

    if (goDown())
    {
        int i = 0;
        soundBuzzer(scrollFrequency, scrollDuration);
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
        soundBuzzer(scrollFrequency, scrollDuration);
        for (int j = 0; j < 8; j++)
        {
            if (dataRateValues[j] == currentSampleRate)
                i = j;
        }
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
 * @brief Performs preliminary control checks.
 *
 * @return true if the preliminary control checks pass, false otherwise.
 */

float currentFactor()
{
    float factor;
    switch (currentChannel)
    {
    case VOLTAGE:
        factor = FACTOR_V;
        break;

    case CURRENT:
        factor = FACTOR_I;
        break;

    case RESISTANCE:
        factor = FACTOR_R;
        break;
    }
    return factor;
}

boolean preliminaryControl()
{
    boolean controlResult = false;
    String message = "Current measure: " + currentChannelString + "\n" + "Gain: " + String(K_value, 30) + "\n" + "Offset: " + String(O_value) + "\n" + "Array length (Sample rate): " + String(measurement.getLength()) + "\n";

    switch (currentMode)
    {
    case SD_ONLY:
        controlResult = initializeSDcard();
        message = message + "Factor: " + String(currentFactor()) + "\n";
        file = SD.open("/dataStorage.ds32", FILE_APPEND);
        file.print(message);
        file.print(getTimeStamp() + " ");
        break;

    case SERIAL_ONLY:
        char serial;
        Serial.println("Write 'F' and send to start the serial acquisition: ");
        waitSerialGraphic();
        serialWaitingTime = time_now = millis();

        while (time_now - serialWaitingTime < TIMEOUT)
        {

            if (Serial.available() > 0)
            {
                serial = Serial.read();
                Serial.println(serial, HEX);
                if (serial == 'F')
                {
                    controlResult = true;
                    Serial.println("START");
                    Serial.println(currentChannelString);
                    Serial.println(K_value, 35);
                    Serial.println(O_value, 35);
                    Serial.println(currentSampleRate);
                    Serial.println(currentFactor());
                    delayMicroseconds(100);
                    break;
                }
            }
            time_now = millis();
        }

        if (serial != 'F')
            Serial.println("Expired time: no valid response received");
        break;

    default:
        controlResult = true;
        break;
    }

    // TODO update error graphic message
    if (!controlResult)
    {
        errorMessageGraphic(currentMode);
        soundBuzzer(1000, 2000);
        delay(3000);
    }

    return controlResult;
}

/**
 * @brief Calculates the coefficient.
 *
 * This function calculates the coefficient based on certain criteria.
 *
 * @return The calculated coefficient.
 */
float calculateCoefficient()
{
    float gain;
    switch (currentChannel)
    {
    case VOLTAGE:
        gain = multiplier_V;
        break;
    case CURRENT:
        gain = multiplier_I;
        break;
    case RESISTANCE:
        gain = multiplier_R;
        break;
    default:
        gain = 1;
        break;
    }
    return gain;
}

/**
 * Calculates the offset.
 *
 * @return The calculated offset as a float value. It is used to calculate the voltage, current, and resistance values.
 */
float calculateOffset()
{
    float offset;
    switch (currentChannel)
    {
    case VOLTAGE:
        offset = 0;
        break;
    case CURRENT:
        offset = 0;
        break;
    case RESISTANCE:
        offset = 0;
        break;
    default:
        offset = 0;
        break;
    }
    return offset;
}

float conversionMeasurement()
{
    float measure;
    switch (currentChannel)
    {
    case VOLTAGE:
        measure = (measurement.getMean() * K_value * FACTOR_V) - O_value;
        break;
    case CURRENT:
        measure = (sqrt(measurement.getMean()) * K_value * FACTOR_I) - O_value;
        break;
    case RESISTANCE:
        measure = FACTOR_R * 3.3 / (measurement.getMean() * K_value) - FACTOR_R;
        break;
    default:
        measure = 0;
        break;
    }
    return measure;
}

/**
 * @brief Sets up the ADC configuration and initializes necessary variables.
 *
 * This function attaches an interrupt to the ALERT_PIN, sets the sample rate and channel,
 * calculates the gain and offset values, and initializes the Measurement object.
 * If the current mode is SERIAL_ONLY, it waits for the 's' character to be received from the serial monitor.
 *
 * @note This function assumes that the NewDataReadyISR function is defined elsewhere.
 */
void adcSetup()
// TODO pass the conversion value to labview<
{
    Serial.println("\n\n\n\n-----------------------------");
    Serial.println("ENTERED IN ADC SETUP\n\n\n\n");
    // We get a falling edge every time a new sample is ready.
    attachInterrupt(digitalPinToInterrupt(ALERT_PIN), NewDataReadyISR, FALLING);
    Serial.println("Interrupt attached (falling edge for new data ready)))");
    setRate(currentSampleRate);
    setChannel();

    measurement.setLength(currentSampleRate);

    K_value = calculateCoefficient();
    O_value = calculateOffset();

    Measurement measurement(currentSampleRate);

    loggerGraphic(getTimeStamp(), 0);
}

void loggerActSD()

{

    if (!new_data)
    {
        return;
    }

    if (!measurement.isArrayFull())
    {
        measurement.insertMeasurement(ads.getLastConversionResults());
        file.print(measurement.getLastMeasurement());
        file.print(" ");
    }
    else
    {
        currentTime = getTimeStamp();
        file.print("\n" + currentTime + " ");
        file.close();
        measurement.setArrayFull(false);
        // appendFile(SD, "/dataStorage.ds32", "\n" + currentTime + "\n");
        loggerGraphic(currentTime, conversionMeasurement());
        file = SD.open("/dataStorage.ds32", FILE_APPEND);
    }
    new_data = false;
}

void loggerActSerial()
{
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
        // Serial.println(measurement.getLastMeasurement());
        Serial.write(0xCC);                                           // Start byte
        Serial.write((measurement.getLastMeasurement() >> 8) & 0xFF); // High byte
        Serial.write(measurement.getLastMeasurement() & 0xFF);        // Low byte
    }
    else
    {
        measurement.setArrayFull(false);
        Serial.println(getTimeStamp());
        loggerGraphic(getTimeStamp(), conversionMeasurement());
    }
    new_data = false;
}

void loggerActDisplay()
{
    if (!new_data)
    {
        // Serial.println("No new data ready!");
        return;
    }

    // Serial.println("New data ready!");

    if (!measurement.isArrayFull())
    {
        measurement.insertMeasurement(ads.getLastConversionResults());
    }
    else
    {
        measurement.setArrayFull(false);
        loggerGraphic(getTimeStamp(), conversionMeasurement());
    }

    new_data = false;
}
