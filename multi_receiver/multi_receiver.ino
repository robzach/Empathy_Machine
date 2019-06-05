/*
   Radio receiver: polls two other radios and collects data from them

   The remote radios have addresses 00001 and 00002 and each will
   transmit an array of two integers in their "acknowledge payload"

   Uses NRF24L01 transceiver module to repeatedly receive remote analog data.

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

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int CE_PIN = 7;
const int CSN_PIN = 8;

const byte NUM_POLLED_RADIOS = 2;
const byte POLLED_RADIO_ADDRESS[NUM_POLLED_RADIOS][5] = {
  // each polled radio has a different address that's 5 bytes long
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 2}
};

RF24 radio(CE_PIN, CSN_PIN);

const char DATA_TO_SEND[2] = "1";
int ackData[2] = { -1, -1}; // to hold the two values coming from the polled radios

unsigned long prevMillis;
const unsigned long POLLING_INTERVAL = 10; // milliseconds between polling

void setup() {
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.enableAckPayload();
  radio.setRetries(3, 5); // delay, count

  Serial.begin(9600);
}

void loop() {
  if (millis() - prevMillis >= POLLING_INTERVAL) {
    int receivedData[4] = {-1, -1, -1, -1};
    // call each radio in turn
    for (byte n = 0; n < NUM_POLLED_RADIOS; n++) {

      // open the writing pipe with the address of a polled radio
      radio.openWritingPipe(POLLED_RADIO_ADDRESS[n]);

      if ( radio.write(&DATA_TO_SEND, sizeof(DATA_TO_SEND)) ) {
        if ( radio.isAckPayloadAvailable() ) {
          radio.read(&ackData, sizeof(ackData));
          int index = n*2;
          receivedData[index] = ackData[0];
          receivedData[index+1] = ackData[1];
        }
        else {
          Serial.println("  Acknowledge but no data ");
        }
      }
      else {
        Serial.println("  Tx failed");
      }
    }
    
    for (int i = 0; i<4; i++){
      Serial.print(receivedData[i]);
      Serial.print('\t');
      if (i == 3) Serial.println();
    }
    
    prevMillis = millis();
  }
}
