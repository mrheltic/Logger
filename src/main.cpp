  #include <Arduino.h>  
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #include <Adafruit_ADS1X15.h> 
  #define OLED_RESET -1  //probably, shares the RST with ESP32


  //DECLARING VARIABLES FOR SCREEN
  #define SCREEN_WIDTH 128  
  #define SCREEN_HEIGHT 64
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //declaring display object

  //DECLARING VARIABLES FOR BUTTONS
  #define BUTTON_1 6
  #define BUTTON_2 7
  #define BUTTON_3 8

  //DECLARING VARIABLES FOR ADC
  Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
  //float voltage = 0; //voltage read from ADC

  void setup() {

    //INITIALIZING SERIAL MONITOR
    Serial.begin(115200);

    while (!Serial) {

      ; // wait for serial port to connect. Needed for native USB port only

    }

    Serial.println("Initializing...");

    /*
    //INITIALIZING BUTTONS
    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);
    pinMode(BUTTON_3, INPUT);
    */

    Serial.println("Buttons initialized");



    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

      Serial.println(F("SSD1306 allocation failed"));

      for (;;)
        ;  // Don't proceed, loop forever
    
    }

    //INITIALIZING SCREEN
    Serial.println("Display initialized");
    display.clearDisplay();

  

    //DISPLAYING TEXT ON SCREEN
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);

    display.println("Display initialized");
    Serial.println("Printing on screen");

    display.display();

    //INITIALIZING ADC
    ads.begin();

    Serial.println("ADC initialized");
    display.println("ADC initialized");
    
    display.display();

    



  }
  void loop() {


  }
