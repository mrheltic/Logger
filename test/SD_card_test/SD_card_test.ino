#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SD.h>
String msg = "";
unsigned long time_now, time_old = 0;

float valsum=0;
File file;
int i=0;
Adafruit_ADS1115 ads;
int count=0;
int16_t arrayRead[470];

void setup() {
  //Serial.begin(9600);
  ads.setGain(GAIN_TWOTHIRDS);
  ads.setDataRate(470);
  ads.begin();
  Serial.begin(115200);

  if (!SD.begin(10)) {
    Serial.println("SD error");
    return;
  }
  Serial.println("SD ready");

  if (SD.exists("LOG_ads.txt")) {
    SD.remove("LOG_ads.txt");
  }

  Serial.println("LOG_ads.txt add");

  
}

void loop() {
  //int val = ads.readADC_SingleEnded(0);
  arrayRead[count]=ads.readADC_SingleEnded(0);

  /*
  Serial.print(val);
  Serial.print(" ");
  Serial.println(i);
  */

  //valsum=valsum+arrayRead[count];
  
  count++;

  if (i==ads.getDataRate()) {
  
  //Serial.println("Ciao_______________________________");
  time_now = millis();
  Serial.print("ci ho messo: ");
  Serial.println(time_now-time_old);
  time_old=time_now;

  for (int count=0; count<ads.getDataRate();count++){
    Serial.print(arrayRead[count]);
  }
  Serial.println();
  i=0;
  //Serial.print("media: ");
  //Serial.println(valsum/ads.getDataRate());
  count=0;
  //valsum=0;
  }

  i++;
  //delay(100);
}

