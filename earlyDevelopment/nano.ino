//------------------------------------------------------------------------------
// NANO_GET_TIME_ESP
//
//
// Author: MArkos Frazzer    Date: 2019-08-21
//
//------------------------------------------------------------------------------
//
//
//------------------------------------------------------------------------------

#include <SoftwareSerial.h>

#define LED 13
#define WAKEUP A4
#define INPUT_PIN 2


//------------------------------------------------------------------------------
// FXN HEADERS FXN HEADERS FXN HEADERS FXN HEADERS FXN HEADERS FXN HEADERS 
//------------------------------------------------------------------------------
void buttonPushed();


// Define some global variables
bool timeSet = false;

int debounce = 5;
uint32_t tStart = 0;
uint32_t epoch = 0;
uint8_t inBuffer[4] = {0};
uint16_t bufIndx = 0;

uint32_t wakeESPInterval = 20000;
uint32_t previouslyWaked = 0;



// Setup an instance of Software Serial
SoftwareSerial querySerial(A2, A3); // RX, TX 

//------------------------------------------------------------------------------
// SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP
//------------------------------------------------------------------------------
void setup() {

  // Setup pins
  pinMode(LED, OUTPUT);
  pinMode(WAKEUP, OUTPUT);
  pinMode(INPUT_PIN, INPUT);
  
  digitalWrite(WAKEUP, HIGH);
  digitalWrite(LED, HIGH);


  // Setup serial
  Serial.begin(115200);
  Serial.println();

  Serial.print(" //-------------------------------------------------- \n");
  Serial.print(" // COMMUNICATION \n");
  Serial.print(" //-------------------------------------------------- \n\n");

  // Setup Software Serial
  querySerial.begin(57600);  // Baud Rate for ESP
   
  digitalWrite(LED, LOW);

  digitalWrite(WAKEUP, LOW);
  delay(1);
  digitalWrite(WAKEUP, HIGH);
    
  while (!timeSet) {
    if (querySerial.available() >= 4) {

      tStart = millis();
  
      Serial.print("\n ");
  
      for (int i = 0; i <= 3; i++) {
        inBuffer[i] = querySerial.read();
        Serial.print(inBuffer[i], HEX);
        Serial.print("\t");
      
     }
    
      epoch = (uint32_t)inBuffer[3] << 0 | (uint32_t)inBuffer[2] << 8 | (uint32_t)inBuffer[1] << 16 | (uint32_t)inBuffer[0] << 24;
      
      Serial.print("\n New epoch: ");
      Serial.print(epoch);
      Serial.print("\n");
      Serial.print(" That took: ");
      Serial.print(millis() - tStart);

      timeSet = true;
     
    }
  }
}


//------------------------------------------------------------------------------
// LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
//------------------------------------------------------------------------------
void loop() {

  if (millis() - previouslyWaked >= wakeESPInterval) {
    previouslyWaked = millis();
    
    digitalWrite(WAKEUP, LOW);
    delay(1);
    digitalWrite(WAKEUP, HIGH);
    
    Serial.print("\n Triggered!");
    
    
  }

  if (querySerial.available() >= 4) {

    tStart = millis();

    Serial.print("\n ");

    for (int i = 0; i <= 3; i++) {
      inBuffer[i] = querySerial.read();
      Serial.print(inBuffer[i], HEX);
      Serial.print("\t");
      
    }
    
    epoch = (uint32_t)inBuffer[3] << 0 | (uint32_t)inBuffer[2] << 8 | (uint32_t)inBuffer[1] << 16 | (uint32_t)inBuffer[0] << 24;
    
    Serial.print("\n New epoch: ");
    Serial.print(epoch);
    Serial.print("\n");
    Serial.print(" That took: ");
    Serial.print(millis() - tStart);
     
  }
    
}



//------------------------------------------------------------------------------
void buttonPushed() {

  delay(debounce);

  digitalWrite(WAKEUP, LOW);
  delay(1);
  digitalWrite(WAKEUP, HIGH);

  Serial.print("\n Triggered!");

  return;

}
