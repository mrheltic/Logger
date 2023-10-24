#include <Arduino.h>
//#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4

//DECLARING VARIABLES FOR SCREEN
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//DECLARING VARIABLES FOR BUTTONS
#define BUTTON_1 12
#define BUTTON_2 18
#define BUTTON_3 19

//DECLARING VARIABLES FOR SD CARD
#define SD_CS 7




void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //INITIALIZING SCREEN

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();

  //INITIALIZING BUTTONS
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);

  //INITIALIZING SD CARD
  /*if(!SD.begin(SD_CS)) {
    Serial.println(F("SD card failed"));
    for(;;);
  }
  */

  //DISPLAYING TEXT ON SCREEN
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Fattuale");
  display.display();

}

void loop() {
  // put your main code here, to run repeatedly:
}
