const int SPKRPIN = 9;
const int YPIN = A0;

void setup() {
  pinMode(SPKRPIN, OUTPUT);
  pinMode(YPIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  int yVal  = analogRead(YPIN);

  int toneVal = map( yVal, 250, 400, 200, 1000);

  tone (SPKRPIN, toneVal);

  delay(1);
}
