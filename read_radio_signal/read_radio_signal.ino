/*
 * Receive signal via radio
 * 
 * Uses NRF24L01 transceiver module to repeatedly receive remote data
 * 
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
 * modified from code by: Alton Olson, Vicky Zhou, Seema Kamath
 * 
 * Robert Zacharias 5-24-19
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int RADIO_CE_PIN = 7;
const int RADIO_CSN_PIN = 8;

RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();

}

void loop() {
  // get radio input from transmitter
  if (radio.available()) {
    int readVal;
    radio.read(&readVal, sizeof(readVal));
    Serial.println(readVal);
  }
}
