//------------------------------------------------------------------------------
// Serial Variable Blip - Slave
//
//
// Author: MArkos Frazzer       Date: 2019-07-12
//
//------------------------------------------------------------------------------
// Its just some UART stuff
//
//------------------------------------------------------------------------------


#include <SoftwareSerial.h>
#include <Time.h>
#include <TimeLib.h>

#define LED 13


// Global Variables
int interval = 1000;
uint32_t previousMillis = 0;

uint32_t tStart = 0;

uint32_t epoch = 1576634084;
uint8_t sendBytes[4] = {0};


// Set up an instance of softwareSerial
SoftwareSerial querySerial(4, 5);  // (rx, tx)


//------------------------------------------------------------------------------
// SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP
//------------------------------------------------------------------------------
void setup() {

  // Setup pinmodes
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Setup serial with computer
  Serial.begin(115200);
  Serial.print(" //-------------------------------------------------- \n");
  Serial.print(" // COMMUNICATION \n");
  Serial.print(" //-------------------------------------------------- \n\n");

  // Setup SoftwareSerial
  querySerial.begin(57600);

  setTime(epoch);

  Serial.print("\n Epoch is: ");
  Serial.print(epoch);
  Serial.print("\t");
  Serial.print(epoch, HEX);



//  newEpoch = (uint32_t)sendBytes[3] << 0 | (uint32_t)sendBytes[2] << 8 | (uint32_t)sendBytes[1] << 16 | (uint32_t)sendBytes[0] << 24;

//  newEpoch += (uint32_t)sendBytes[0] << 24;
//  newEpoch += (uint32_t)sendBytes[1] << 16;
//  newEpoch += (uint32_t)sendBytes[2] << 8;
//  newEpoch += (uint32_t)sendBytes[3];
//  
//  for (int i = 0; i <= 3; i++) {
//    newEpoch = sendBytes[i];
//    newEpoch << 8;
//    
//  }
  
}


//------------------------------------------------------------------------------
// LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
//------------------------------------------------------------------------------
void loop() {

  if (millis() - previousMillis >= interval) {

    previousMillis = millis();

    epoch =  now();
    Serial.print("\n\n Epoch is: ");
    Serial.print(epoch);
    Serial.print("\t");
    Serial.print(epoch, HEX);
    
    sendBytes[0] = epoch >> 24;
    sendBytes[1] = epoch >> 16;
    sendBytes[2] = epoch >> 8;
    sendBytes[3] = epoch >> 0;
  
    Serial.print("\n Byte 1: ");
    Serial.print(sendBytes[0], HEX);
    Serial.print("\n Byte 2: ");
    Serial.print(sendBytes[1], HEX);
    Serial.print("\n Byte 3: ");
    Serial.print(sendBytes[2], HEX);
    Serial.print("\n Byte 4: ");
    Serial.print(sendBytes[3], HEX);

    tStart = millis();

    for (int i = 0; i <= 3; i++) {
      querySerial.write(sendBytes[i]);
      
    }

    Serial.print("\n Sending that took: ");
    Serial.print(millis() - tStart);
    
  }

}

//------------------------------------------------------------------------------
