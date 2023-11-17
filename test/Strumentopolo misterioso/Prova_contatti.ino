void setup() {
  Serial.begin(9600);
pinMode(2, INPUT_PULLUP);
pinMode(4, INPUT_PULLUP);
pinMode(7, INPUT_PULLUP);
}

void loop() {
  Serial.println("pulsante 1: ");
Serial.print(digitalRead(2));  
  Serial.println("pulsante 2: ");
Serial.print(digitalRead(4));  
  Serial.println("pulsante 3: ");
Serial.print(digitalRead(7));  

}
