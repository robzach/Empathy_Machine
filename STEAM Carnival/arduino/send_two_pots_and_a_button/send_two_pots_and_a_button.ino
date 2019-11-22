const int POTPIN = A0;
const int SLIDEPIN = A2;
const int BUTTONPIN = 3;
const int SWITCHPIN = 5;


void setup() {
  pinMode(POTPIN, INPUT);
  pinMode(SLIDEPIN, INPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(SWITCHPIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  Serial.print(analogRead(POTPIN));
  Serial.print(",");
  Serial.print(analogRead(SLIDEPIN));
  Serial.print(",");
  Serial.print(!digitalRead(BUTTONPIN));
  Serial.print(",");
  Serial.print(!digitalRead(SWITCHPIN));
  Serial.println(",");
  delay(10);
}
