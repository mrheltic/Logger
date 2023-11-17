//8월 16일

enum { B = 2, G, R, BUTTON};
int index;

void setup()
{
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop()
{
  while(digitalRead(BUTTON) == HIGH);
  switch (index)
  {
    case 0:
      RGB(HIGH, HIGH, LOW);
      break;
    case 1:
      RGB(HIGH, LOW, HIGH);
      break;
    case 2:
      RGB(LOW, HIGH, HIGH);
      break;
    case 3:
      RGB(LOW, LOW, LOW);
      break;
  }
  index = index == 3 ? 0 : index + 1;
  delay(100);
}

void RGB(int r, int g, int b)
{
      digitalWrite(R, r);
      digitalWrite(G, g);
      digitalWrite(B, b);
}
