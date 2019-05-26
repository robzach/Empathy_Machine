/*
 * Receive signal via radio and emit MIDI signals to an attached computer
 * 
 * Uses NRF24L01 transceiver module to receive remote data
 * 
 * Must use a Leonardo, Micro, or other Arduino with native USB capability
 * to transmit MIDI signals to the attached computer
 * 
 * 
 * radio pin    Arduino Micro pin
 * VCC          3.3V
 * GND          GND
 * CE           7
 * CSN          8
 * MOSI         MO
 * MISO         MI
 * SCK          SCK
 * 
 * modified from code by: Alton Olson, Vicky Zhou, Seema Kamath
 * 
 * Robert Zacharias 5-26-19
 */

#include "MIDIUSB.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// pin values
const int RADIO_CE_PIN = 7;
const int RADIO_CSN_PIN = 8;

// values used for auto-ranging input seen
int highest = 0;
int lowest = 1023;

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

    // load received value into a local variable
    int readVal;
    radio.read(&readVal, sizeof(readVal));

    // simple autorange for normalizing values
    if (readVal > highest) highest = readVal;
    if (readVal < lowest) lowest = readVal;

    // scale observed output to 0–255 range
    int outVal = map(readVal, lowest, highest, 0, 127);

    // transmit this value via MIDI
    controlChange(0, 10, outVal);
    
    Serial.println(outVal);
  }
}


void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
