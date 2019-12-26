//------------------------------------------------------------------------------
// Drive Seven Seg
//
//
// Author: MArkos Frazzer    Date: 2019-12-26
//
//------------------------------------------------------------------------------
//
// Use port manipulation to drive seven segment display
// When declaring ports as input/output, leave any hardware reserved bits as 0
// Port D for digits 1 -> 4, segA, and segB
// Port B for segC, segD, segE, segF, segG, and decimal point
// Port C for colon and apostrophe
//
//------------------------------------------------------------------------------

// Declare mah functions
void digit(int a, bool decimal);
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
uint16_t period = 1;
uint32_t previousMillis = 0;
uint8_t binShifter = 0;
uint8_t digitBin = 0b00000100;

uint16_t displayInt = 0;
uint8_t thousandths = 0;
uint8_t hundredths = 0;
uint8_t tenths = 0;
uint8_t oneths = 0;

//------------------------------------------------------------------------------
// SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP
//------------------------------------------------------------------------------
void setup() {

  Serial.begin(115200);

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
  DDRC = DDRC | 0b00111111;
  PORTC = 0b00000000;

  
}


//------------------------------------------------------------------------------
// LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
//------------------------------------------------------------------------------
void loop() {

  if (Serial.available() > 0) {
    displayInt = Serial.parseInt();
    Serial.print("\n Got an integer: ");
    Serial.print(displayInt);
    Serial.print("\n Thousandths: ");
    Serial.print((displayInt / 1000) % 10);
    thousandths = displayInt / 1000 % 10;
    Serial.print("\t Hundredths: ");
    Serial.print((displayInt / 100) % 10);
    hundredths = displayInt / 100 % 10;
    Serial.print("\t Tenths: ");
    Serial.print((displayInt / 10) % 10);
    tenths = displayInt / 10 % 10;
    Serial.print("\t Oneths: ");
    Serial.print(displayInt % 10);
    oneths = displayInt % 10;
    
  }

  if (millis() - previousMillis >= period) {
    previousMillis = millis();
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