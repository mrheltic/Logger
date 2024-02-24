#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BusIO_Register.h>



// Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1115 ads;     /* Use this for the 12-bit version */

// Pin connected to the ALERT/RDY signal for new sample notification.
constexpr int READY_PIN = 14;

// This is required on ESP32 to put the ISR in IRAM. Define as
// empty for other platforms. Be careful - other platforms may have
// other requirements.
#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

unsigned long time_now, time_old = 0;

float valsum=0;
int i=0;
int count=0;
int arrayRead[860];
float sumValue=0;
String stringValue="";


volatile bool new_data = false;
void IRAM_ATTR NewDataReadyISR() {
  new_data = true;
}

void printValue(int value,int i){
  Serial.print(value);
  Serial.print(" ");
  Serial.println(i);
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Hello!");

  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  //ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  ads.setDataRate(RATE_ADS1115_860SPS);

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  pinMode(READY_PIN, INPUT);
  // We get a falling edge every time a new sample is ready.
  attachInterrupt(digitalPinToInterrupt(READY_PIN), NewDataReadyISR, FALLING);

  // Start continuous conversions.
  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, true);
}

void loop(void)
{
  // If we don't have new data, skip this iteration.
  if (!new_data) {
    return;
  }

  arrayRead[count]=ads.getLastConversionResults();

  new_data = false;

  count++;

  if (i==860) {
  
  //Serial.println("Ciao_______________________________");
  time_now = millis();
  Serial.print("ci ho messo: ");
  Serial.println(time_now-time_old);
  time_old=time_now;

  Serial.println();
  i=0;
  //Serial.print("media: ");
  //Serial.println(valsum/ads.getDataRate());
  //valsum=0;
  for (int count=0; count<860;count++){
  sumValue=sumValue+arrayRead[count];
  Serial.print(arrayRead[count]);
  }
  Serial.print("media: ");
  Serial.println(sumValue/860);
  Serial.println(stringValue);
  count=0;
  sumValue=0;
  stringValue="";
  }

  i++;

}
