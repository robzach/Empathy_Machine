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

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int XPIN = A0;
const int YPIN = A1;
const int ZPIN = A2;

const int RADIO_CE = 7;
const int RADIO_CSN = 8;

RF24 radio(RADIO_CE, RADIO_CSN);
const byte address[6] = "00001";

void setup() {
  // set up radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();

  pinMode(XPIN, INPUT);
  pinMode(YPIN, INPUT);
  pinMode(ZPIN, INPUT);
}

void loop() {
  // read analog values
  int reading[3] = {
    analogRead(XPIN),
    analogRead(YPIN),
    analogRead(ZPIN)
  };

  // transmit values
  radio.write(&reading, sizeof(reading));
  delay(1);
}
