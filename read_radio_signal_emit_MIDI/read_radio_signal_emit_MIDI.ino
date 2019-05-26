/*
   Receive signal via radio and emit MIDI signals to an attached computer

   Uses NRF24L01 transceiver module to receive remote data

   Must use a Leonardo, Micro, or other Arduino with native USB capability
   to transmit MIDI signals to the attached computer


   radio pin    Arduino Micro pin
   VCC          3.3V
   GND          GND
   CE           7
   CSN          8
   MOSI         MO
   MISO         MI
   SCK          SCK

   modified from code by: Alton Olson, Vicky Zhou, Seema Kamath

   Robert Zacharias 5-26-19
*/

#include "MIDIUSB.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// pin values
const int RADIO_CE_PIN = 7;
const int RADIO_CSN_PIN = 8;

// length of data array to receive via radio
const int ARRAY_LENGTH = 4;

// values used for auto-ranging input seen
int highest[ARRAY_LENGTH] = {0, 0, 0, 0};
int lowest[ARRAY_LENGTH] = {1023, 1023, 1023, 1023};

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

    // load received values into a local array
    int readVal[ARRAY_LENGTH];
    radio.read(&readVal, sizeof(readVal));

    // simple autorange for normalizing values
    for (int i = 0; i < sizeof(readVal) / sizeof(readVal[0]); i++) {
      if (readVal[i] > highest[i]) highest[i] = readVal[i];
      if (readVal[i] < lowest[i]) lowest[i] = readVal[i];
    }

    // scale observed output to 0–127 MIDI range
    int outVal[ARRAY_LENGTH];
    for (int i = 0; i < sizeof(readVal) / sizeof(readVal[0]); i++) {
      outVal[i] = map(readVal[i], lowest[i], highest[i], 0, 127);
    }

    // transmit these values via three different MIDI control channels
    controlChange(0, 10, outVal[0]);
    controlChange(0, 11, outVal[1]);
    controlChange(0, 12, outVal[2]);
    controlChange(0, 13, outVal[3]);

    // Serial debugging
    for (int i = 0; i < sizeof(readVal) / sizeof(readVal[0]); i++) {
      Serial.print(outVal[i]);
      Serial.print(',');
      if (i == (sizeof(readVal) / sizeof(readVal[0])) - 1) Serial.println();
    }
  }
}


void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
