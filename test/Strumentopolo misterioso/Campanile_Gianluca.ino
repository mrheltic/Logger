#include <DS1302.h>

int pul1 = 11;
int pul2 = 12;
int pul3 = 13;
int R  = 7;
int G = 6;
int B = 5;
int buz = 10;
int campana = 9;
int lamp = 8;
int counter = 0;
int h;
int m;
int Hour;
int Min;
int index;

Time t;
DS1302 rtc(2, 3, 4);

void setup () {
  Serial.begin(9600);
  Serial.println("Campanile carosino, di Gianluca Marzo e Francesco Stasi");

  pinMode(pul1, INPUT_PULLUP);
  pinMode(pul2, INPUT_PULLUP);
  pinMode(pul3, INPUT_PULLUP);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(buz, OUTPUT);
  pinMode(campana, OUTPUT);
  pinMode(lamp, OUTPUT);

  digitalWrite(B, 0);
  digitalWrite(R, 0);
  digitalWrite(G, 0);
  digitalWrite(lamp, 1);
  digitalWrite(campana, 0);

  rtc.halt(false);
  rtc.writeProtect(false);
  h = 22;
  m = 24;
  rtc.setDOW(SATURDAY);
  rtc.setTime(22, 23, 0);
  rtc.setDate(29, 10, 2020);
}

void loop () {
  /*
    Serial.println(rtc.getTimeStr());
    t = rtc.getTime();
    Hour = t.hour;
    Min = t.min;
  */
  while (digitalRead(pul1) == 1);
  switch (index)
  {
    case 0:
      RGB(0, 0, 0);
      break;
    case 1:
      RGB(0, 0, 1);
      break;
    case 2:
      RGB(0, 1, 0);
      break;
    case 3:
      RGB(0, 1, 1);
      break;
    case 4:
      RGB(1, 0, 0);
      break;
    case 5:
      RGB(1, 0, 1);
      break;
    case 6:
      RGB(1, 1, 0);
      break;
    case 7:
      RGB(1, 1, 1);
      break;
    //case 8:
     /* for (int i = 0; i < 256; i++) {
        RGB(255 - i, i, 0);
        delay(10);
      }
      for (int i = 0; i < 256; i++) {
        RGB(0, 255 - i, i);
        delay(10);
      }
      for (int i = 0; i < 256; i++) {
        RGB(i, 0, 255 - i);
        delay(10);
      }*/
      //break;
  }

   index = index == 7 ? 0 : index + 1;
  Serial.print(digitalRead(pul3));
Serial.println(digitalRead(pul2));
Serial.print("index=");
Serial.println(index);
  if (digitalRead(pul3) == 0 && digitalRead(pul3) == 0) {
    RGB(1, 0, 1);
    digitalWrite(campana, 1);
    delay (10000);
    RGB(0, 0, 0);
    digitalWrite(campana, 0);
  }
  /*
    if (Hour== h && Min== m) {

    tone(buz,400,300);
    digitalWrite(campana, 1);
    }
    else {
    digitalWrite(campana, 0);
    }
  */
 
  delay (200);
}

void RGB(int r, int g, int b) {
  digitalWrite(R, r);
  digitalWrite(G, g);
  digitalWrite(B, b);
}
