//------------------------------------------------------------------------------
// Nano Internet Clock
//
//
// Author: MArkos Frazzer    Date: 2019-12-26
//
//------------------------------------------------------------------------------
//
// Use port manipulation to drive seven segment display
// Use SoftwareSerial to communicate with an ESP8266 module to sync time NTP
//
// When declaring ports as input/output, leave any hardware reserved bits as 0
// Port D for digits 1 -> 4, segA, and segB
// Port B for segC, segD, segE, segF, segG, and decimal point
// Port C for colon, apostrophe, and SoftwareSerial
//
//------------------------------------------------------------------------------

#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <SoftwareSerial.h>

// Declare mah functions
void listenSerial();
void digit(int a, bool decimal);
void blipColon();
void zero(bool decimal);
void one(bool decimal);
void two(bool decimal);
void three(bool decimal);
void four(bool decimal);
void five(bool decimal);
void six(bool decimal);
void seven(bool decimal);
void eight(bool decimal);
void nine(bool decimal);
void clearAll();


// Global Variables
// Multiplexing variables
uint16_t displayPeriod = 1;
uint32_t previousDisplayMillis = 0;
uint8_t binShifter = 0;
uint8_t digitBin = 0b00000100;

// Time data variables
uint8_t updateTimePeriod = 100;
uint32_t previousTimeMillis = 0;
uint16_t displayInt = 0;
uint16_t previousDisplayInt = 0;
uint8_t thousandths = 0;
uint8_t hundredths = 0;
uint8_t tenths = 0;
uint8_t oneths = 0;

// Colon heartbeat variables
bool LEDState = false;
uint16_t LEDInterval = 0; // Period of execution, changes depending on LEDState
int16_t tOn = 1000;  // LED on time
int16_t tOff = 50;  // LED off time
uint32_t LEDPreviousMillis = 0;

// Communication epoch variables
bool firstSet = false;
uint32_t wakeESPInterval = 480000;
uint32_t previouslyWaked = 0;
uint32_t epoch = 0;
uint32_t tStart = 0;
uint8_t inBuffer[4] = {0};
uint16_t bufIndx = 0;


// Set up some timezone structs
// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPT(usPDT, usPST);  // Constructor

// Setup an instance of Software Serial
SoftwareSerial querySerial(A2, A3); // RX, TX 

//------------------------------------------------------------------------------
// SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP
//------------------------------------------------------------------------------
void setup() {

  // Setup serial
  Serial.begin(115200);
  Serial.println();

  Serial.print(" //-------------------------------------------------- \n");
  Serial.print(" // NANO INTERNET CLOCK \n");
  Serial.print(" //-------------------------------------------------- \n\n");

  // Port D maps to digital pins 0 -> 7
  // D0 and D1 (low bits) are reserved as hardware serial, don't use
  // d1 = bit3, d2 = bit4, d3 = bit5, d4 = bit6, segA = bit7, segB = bit8
  DDRD = DDRD | 0b11111100;
  PORTD = 0b00000000;

  // Port B maps to digital pins 8 -> 13
  // High bits are reserved for crystal oscillator, don't use
  // segC = bit1, segD = bit2, segE = bit3, segF = bit4, segG = bit5, dp = bit6
  DDRB = DDRB | 0b00111111;
  PORTB = 0b00000000;

  // Port C maps to analogue pins 0 -> 5, dont touch the high bits
  // We will use A2 and A3 for softwareSerial, so leave those as input for now
  // A4 will be the wakeUp pin for the ESP, set it to output
  // A0 = bit1 --> A5 = bit6
  DDRC = DDRC | 0b00010011;
  PORTC = 0b00010000;

  // Setup Software Serial
  querySerial.begin(57600);  // Baud Rate for ESP

  // Get the epoch for the first time (or never exit the setup())!
  PORTC = 0b00000000;
  delay(1);
  PORTC = 0b00010000;
  while (!firstSet) {
    listenSerial();

  }
}


//------------------------------------------------------------------------------
// LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
//------------------------------------------------------------------------------
void loop() {

  // Keep the display variables updated
  if (millis() - previousTimeMillis >= updateTimePeriod) {
    previousTimeMillis = millis();
    displayInt = (hour() * 100) + minute();

    if (displayInt != previousDisplayInt) {
      previousDisplayInt = displayInt;

      thousandths = displayInt / 1000 % 10;
      hundredths = displayInt / 100 % 10;
      tenths = displayInt / 10 % 10;
      oneths = displayInt % 10;
      
    }
  }


  // Actually multiplex the display
  if (millis() - previousDisplayMillis >= displayPeriod) {
    previousDisplayMillis = millis();
    clearAll();
    
    digitBin = 0b00000100 << binShifter;

    if (digitBin == 4) {
      digit(thousandths, false);
      
    }

    if (digitBin == 8) {
      digit(hundredths, false);
      
    }

    if (digitBin == 16) {
      digit(tenths, false);
      
    }

    if (digitBin == 32) {
      digit(oneths, false);
      
    }

    PORTD |= digitBin;

    if (binShifter < 3) {
      binShifter++;
      
    } else {
      binShifter = 0;
      
    }
  }

  // Wake up the ESP to re-sync
  if (millis() - previouslyWaked >= wakeESPInterval) {
    previouslyWaked = millis();
    
    PORTC = 0b00000000;
    delay(1);
    PORTC = 0b00010000;
    
  }

  blipColon();
  listenSerial();
  
}


//------------------------------------------------------------------------------
void listenSerial() {
  
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

    setTime(usPT.toLocal(epoch));
    displayInt = (hour() * 100) + minute();
    Serial.println(displayInt);

    if (!firstSet) {
      firstSet = true;
      
    }
  }
}

//------------------------------------------------------------------------------
void digit(int a, bool decimal) {
  switch(a) {
    case 0:
      zero(decimal);
      break;
    case 1:
      one(decimal);
      break;
    case 2:
      two(decimal);
      break;
    case 3:
      three(decimal);
      break;
    case 4:
      four(decimal);
      break;
    case 5:
      five(decimal);
      break;
    case 6:
      six(decimal);
      break;
    case 7:
      seven(decimal);
      break;
    case 8:
      eight(decimal);
      break;
    case 9:
      nine(decimal);
      break;
    default:
      clearAll();
      break;
      
  }
}

//------------------------------------------------------------------------------
void blipColon() {

  if (millis() - LEDPreviousMillis >= LEDInterval) {

    LEDPreviousMillis = millis();
    
    if (LEDState) {
      LEDInterval = tOff;
      LEDState = !LEDState;
      PORTC = 0b00010000;
      
    } else {
      LEDInterval = tOn;
      LEDState = !LEDState;
      PORTC = 0b00010001;
      
    }
  }
}

//------------------------------------------------------------------------------
void zero(bool decimal) {
  // Port D for digits 1 -> 4, segA, and segB
  // Port B for segC, segD, segE, segF, segG, and decimal point
  // For zero, segA, B, C, D, E, F will be LOW
  // segG will be HIGH
  PORTD = 0b00000000;
  PORTB = 0b00010000;

  if (!decimal) {
    PORTB |= 0b00100000;
  }

}

//------------------------------------------------------------------------------
void one(bool decimal) {
  PORTD = 0b01000000;
  PORTB = 0b00011110;

  if (!decimal) {
    PORTB |= 0b00100000;
  }

}

//------------------------------------------------------------------------------
void two(bool decimal) {
  PORTD = 0b00000000;
  PORTB = 0b00001001;

  if (!decimal) {
    PORTB |= 0b00100000;
  }

}

//------------------------------------------------------------------------------
void three(bool decimal) {
  PORTD = 0b00000000;
  PORTB = 0b00001100;

  if (!decimal) {
    PORTB |= 0b00100000;
  }
  
}

//------------------------------------------------------------------------------
void four(bool decimal) {
  PORTD = 0b01000000;
  PORTB = 0b00000110;

  if (!decimal) {
    PORTB |= 0b00100000;
  }
  
}

//------------------------------------------------------------------------------
void five(bool decimal) {
  PORTD = 0b10000000;
  PORTB = 0b00000100;

  if (!decimal) {
    PORTB |= 0b00100000;
  }
  
}

//------------------------------------------------------------------------------
void six(bool decimal) {
  PORTD = 0b10000000;
  PORTB = 0b00000000;

  if (!decimal) {
    PORTB |= 0b00100000;
  }
  
}

//------------------------------------------------------------------------------
void seven(bool decimal) {
  PORTD = 0b00000000;
  PORTB = 0b00011110;

  if (!decimal) {
    PORTB |= 0b00100000;
  }
  
}

//------------------------------------------------------------------------------
void eight(bool decimal) {
  PORTD = 0b00000000;
  PORTB = 0b00000000;

  if (!decimal) {
    PORTB |= 0b00100000;
  }
  
}

//------------------------------------------------------------------------------
void nine(bool decimal) {
  PORTD = 0b00000000;
  PORTB = 0b00000100;

  if (!decimal) {
    PORTB |= 0b00100000;
  }
  
}

//------------------------------------------------------------------------------
void clearAll() {
  PORTD = 0b00000000;
  PORTB = 0b00000000;
  
}

//------------------------------------------------------------------------------