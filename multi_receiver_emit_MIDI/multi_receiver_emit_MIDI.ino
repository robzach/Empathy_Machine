/*
   Radio receiver: polls two other radios and collects data from them
   and emits MIDI data via USB to an attached computer

   For the purpose of training music software on the different signals
   available, pressing a physical button will isolate one MIDI signal to
   the computer at a time, and then return to transmitting all 4.

   The remote radios have addresses 00001 and 00002 and each will
   transmit an array of two integers via their "acknowledge payload"

   Uses NRF24L01 transceiver module.

   radio pin    Arduino Uno/Nano pin    Arduino Micro pin
   VCC          3.3V                    3.3V
   GND          GND                     GND
   CE           7                       7
   CSN          8                       8
   MOSI         11                      MO
   MISO         12                      MI
   SCK          13                      SCK


   adapted from "MultiTxAckPayload" by user Robin2 on Arduino.cc forum
   Robert Zacharias, rz@rzach.me, 6-5-19
*/

#include "MIDIUSB.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// user presses this button to isolate MIDI inputs
const int BUTTON_PIN = 2;
byte MIDImode = 0;
bool oldButtonState = false;

// radio constructor
const int CE_PIN = 7;
const int CSN_PIN = 8;
RF24 radio(CE_PIN, CSN_PIN);

const byte NUM_POLLED_RADIOS = 2;
const byte POLLED_RADIO_ADDRESS[NUM_POLLED_RADIOS][5] = {
  // each polled radio has a different address that's 5 bytes long
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 2}
};

const char DATA_TO_SEND[2] = "1";
int ackData[2] = { -1, -1}; // to hold the two values coming from the polled radios
int receivedData[4] = { -1, -1, -1, -1}; // to hold all four read values

// values used for auto-ranging for MIDI output
int highest[4] = {0, 0, 0, 0};
int lowest[4] = {1023, 1023, 1023, 1023};

unsigned long prevMillis;
const unsigned long POLLING_INTERVAL = 10; // milliseconds between polling

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.enableAckPayload();
  radio.setRetries(3, 5); // delay, count

  Serial.begin(9600);
}

void loop() {
  if (millis() - prevMillis >= POLLING_INTERVAL) {

    pollForRadioData();
    writeToMIDI();

    prevMillis = millis();
  }
  
  bool buttonState = !digitalRead(BUTTON_PIN);
  if (buttonState && !oldButtonState){
    MIDImode++;
    MIDImode %= 5;
    delay(50);
  }
  oldButtonState = buttonState;
  
}

void pollForRadioData() {
  // call each radio in turn
  for (byte n = 0; n < NUM_POLLED_RADIOS; n++) {

    // open the writing pipe with the address of a polled radio
    radio.openWritingPipe(POLLED_RADIO_ADDRESS[n]);

    if ( radio.write(&DATA_TO_SEND, sizeof(DATA_TO_SEND)) ) {
      if ( radio.isAckPayloadAvailable() ) {
        radio.read(&ackData, sizeof(ackData));
        int index = n * 2;
        receivedData[index] = ackData[0];
        receivedData[index + 1] = ackData[1];
      }
      else {
        Serial.println("  Acknowledge but no data ");
      }
    }
    else {
      Serial.println("  Tx failed");
    }
  }

  for (int i = 0; i < 4; i++) {
    Serial.print(receivedData[i]);
    Serial.print('\t');
    if (i == 3) Serial.println();
  }
}

void writeToMIDI() {
  // simple autorange for normalizing values
  for (int i = 0; i < sizeof(receivedData) / sizeof(receivedData[0]); i++) {
    if (receivedData[i] > highest[i]) highest[i] = receivedData[i];
    if (receivedData[i] < lowest[i]) lowest[i] = receivedData[i];
  }

  // scale observed output to 0–127 MIDI range
  int outVal[4];
  for (int i = 0; i < sizeof(receivedData) / sizeof(receivedData[0]); i++) {
    outVal[i] = map(receivedData[i], lowest[i], highest[i], 0, 127);
  }

  switch (MIDImode) {
    case 1: // transmit only first MIDI value
      controlChange(0, 10, outVal[0]);
      break;
    case 2: // transmit only second MIDI value
      controlChange(0, 11, outVal[0]);
      break;
    case 3: // transmit only third MIDI value
      controlChange(0, 12, outVal[0]);
      break;
    case 4: // transmit only fourth MIDI value
      controlChange(0, 13, outVal[0]);
      break;
    case 0:
    default:
      // transmit all four MIDI values
      controlChange(0, 10, outVal[0]);
      controlChange(0, 11, outVal[1]);
      controlChange(0, 12, outVal[2]);
      controlChange(0, 13, outVal[3]);
      break;
  }
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
