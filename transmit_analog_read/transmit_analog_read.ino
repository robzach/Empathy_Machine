/*
 * Transmit analog read signal via radio
 * 
 * Uses NRF24L01 transceiver module to repeatedly transmit analog data.
 * 
 * radio pin    Arduino pin
 * VCC          3.3V
 * GND          GND
 * CE           7
 * CSN          8
 * MOSI         11
 * MISO         12
 * SCK          13
 * 
 * Value observed on analog pin A0 is transmitted.
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

const int ANALOG_READ_PIN = A0;

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
}

void loop() {
  // read analog value
  int reading = analogRead(ANALOG_READ_PIN);

  // transmit value
  radio.write(&reading, sizeof(reading));
  delay(1);
}
