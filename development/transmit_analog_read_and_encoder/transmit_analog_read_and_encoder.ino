/*
 * Transmit analog read signal via radio
 * 
 * Uses NRF24L01 transceiver module to repeatedly transmit analog data.
 * 
 * radio pin    Arduino Uno/Nano pin    Arduino Micro pin
 * VCC          3.3V                    3.3V
 * GND          GND                     GND
 * CE           7                       7
 * CSN          8                       8
 * MOSI         11                      MO
 * MISO         12                      MI
 * SCK          13                      SCK
 *          
 * 
 * Values observed on analog pins A0, A1, and A2 are transmitted.
 * 
 *
 * modified from code by: Alton Olson, Vicky Zhou, Seema Kamath
 * 
 * Robert Zacharias 5-24-19
 * 
 */
 
#include <Encoder.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int XPIN = A0;
const int YPIN = A1;
const int ZPIN = A2;
const int PHOTOPIN = A3;

const int ENC1PIN = 2;
const int ENC2PIN = 3;

const int RADIO_CE = 7;
const int RADIO_CSN = 8;

RF24 radio(RADIO_CE, RADIO_CSN);
const byte address[6] = "00001";

Encoder encoder(ENC1PIN, ENC2PIN);

void setup() {
  // inputs
  pinMode(XPIN, INPUT);
  pinMode(YPIN, INPUT);
  pinMode(ZPIN, INPUT);
  pinMode(PHOTOPIN, INPUT);
  
  // set up radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();

  Serial.begin(9600);
}

void loop() {
  // read encoder
  long encoderPos = encoder.read();
  
  // read analog values
  int reading[4] = {
    analogRead(XPIN),
    analogRead(ZPIN),
    encoderPos,
    analogRead(PHOTOPIN)
  };

  // transmit values
  radio.write(&reading, sizeof(reading));
  delay(1);
}
