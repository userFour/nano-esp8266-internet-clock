//------------------------------------------------------------------------------
// Adafruit HUZZAH8266 NTP Query
//
//
// Author(s): MArkos Frazzer, Ethan, U.J.    Date: 2019-12-26
//
//------------------------------------------------------------------------------
//
// SEE ALL THE LIBRARY DOCUMENTATION LOL
//
//------------------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

#define BLUE_LED 2
#define RED_LED 0
#define NTP_PACKET_SIZE 48


// Function headers
void beginSync();
unsigned long sendNtpPacket(IPAddress& address);
unsigned long getLongFromPacket(byte (&packet)[NTP_PACKET_SIZE], int offset);
void handleNtpPacket(byte (&packet)[NTP_PACKET_SIZE], float ntpDelay);


// Define some constants
const unsigned long SEVENTY_YEARS = 2208988800UL;
// 4294967296 is the maximum integer for an 32 bit integer
// declared as a float so math works :)
const float MAX_INT = 4294967296.0;
const char* NTP_SERVER_NAME = "time.nist.gov";


// Define some variables
char ssid[] = "PHASZNET-2.4G";
char pass[] = "314159pi00";

int packetSize = 0;
int sendMillis = 0;
int timeToReceive = 0;
unsigned int localPort = 2390;
unsigned long epoch = 0;
unsigned long ntpSecs = 0;
unsigned long ntpMillisInt = 0;
float epochMillis = 0.0;
float ntpMillis = 0.0;
float ntpDelay = 0.0;

byte packetBuffer[NTP_PACKET_SIZE];

// SoftwareSerial Variables
uint8_t sendBytes[4] = {0};


// Set up an instance of softwareSerial
SoftwareSerial querySerial(12, 14);  // (rx, tx)


// Set up some WiFi stuff
IPAddress timeServerIP;
WiFiUDP udp;

//------------------------------------------------------------------------------
// SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP SET UP
//------------------------------------------------------------------------------
void setup() {

  // Setup pinmodes
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  // Setup serial
  Serial.begin(115200);
  Serial.println();

  Serial.print(" //-------------------------------------------------- \n");
  Serial.print(" // TIME SYNC \n");
  Serial.print(" //-------------------------------------------------- \n\n");

  // Setup SoftwareSerial
  querySerial.begin(57600);
  
  Serial.print(" Connecting to ");
  Serial.println(ssid);
  Serial.print(" ");
  WiFi.begin(ssid, pass);


  // Connect the WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print connection info
  Serial.println(" WiFi connected");
  Serial.println(" IP address: ");
  Serial.print(" ");
  Serial.println(WiFi.localIP());
  Serial.println(" Starting UDP");
  udp.begin(localPort);
  Serial.print(" Local port: ");
  Serial.print(udp.localPort());
  Serial.print("\n\n");

}


//------------------------------------------------------------------------------
// LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP
//------------------------------------------------------------------------------
void loop() {

  beginSync();

  digitalWrite(BLUE_LED, HIGH);

  ESP.deepSleep(0);  // Sleep untill RST is pulled low by the Nano

}

//------------------------------------------------------------------------------
void beginSync() {

  WiFi.hostByName(NTP_SERVER_NAME, timeServerIP); 

  sendMillis = millis();
  
  sendNtpPacket(timeServerIP); 
  
  packetSize = udp.parsePacket();
  
  while (!packetSize) {
    Serial.println(" No packet yet");
    packetSize = udp.parsePacket();
    delay(20);
    
    if((millis() - sendMillis) > 5000) {
        Serial.println(" Failure: No packet received within 5 seconds");
        break;
        
    }
  }
  
  Serial.print(" Packet received of length ");
  Serial.print(packetSize);
  
  Serial.print("\n Time to receive: ");
  timeToReceive = millis() - sendMillis;
  Serial.print(timeToReceive);
  
  // Assume a symmetrical delay
  ntpDelay = (timeToReceive / 2.0) / 1000.0;
  
  // We've received a packet, read the data from it
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  handleNtpPacket(packetBuffer, ntpDelay);
  
}

//------------------------------------------------------------------------------
unsigned long sendNtpPacket(IPAddress& address) {
  // send an NTP request to the time server at the given address
  
  // Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // indicate binary (0b), 00 (no leap warning), 
  // 011 (version 3), 011 (client mode)
  // Use version 3 because code cannot handle 64bit integers
  packetBuffer[0] = 0b00011011;

  // All NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
  
}

//------------------------------------------------------------------------------
void handleNtpPacket(byte (&packet)[NTP_PACKET_SIZE], float ntpDelay) {
  
  // Combine the four bytes into a long integer
  // This is NTP time (seconds since Jan 1 1900) in seconds
  ntpSecs = getLongFromPacket(packetBuffer, 40);

  // this is the NTP time's milliseconds
  ntpMillisInt = getLongFromPacket(packetBuffer, 44);
  ntpMillis = ntpMillisInt / MAX_INT;

  // Now convert NTP time into epoch time
  // Unix time starts on Jan 1 1970. In NTP that's 2208988800 seconds
  // Subtract seventy years from NTP to get epoch, and 
  // account for the delay in receiving the NTP response
  epoch = ntpSecs - SEVENTY_YEARS + floor(ntpMillis + ntpDelay);

  sendBytes[0] = epoch >> 24;
  sendBytes[1] = epoch >> 16;
  sendBytes[2] = epoch >> 8;
  sendBytes[3] = epoch >> 0;
  
  for (int i = 0; i <= 3; i++) {
    querySerial.write(sendBytes[i]);
  
  }
  
}

//------------------------------------------------------------------------------
unsigned long getLongFromPacket(byte (&packet)[NTP_PACKET_SIZE], int offset) {
  
    return packet[offset] << 24 | packet[offset+1] << 16 | packet[offset+2] << 8 | packet[offset+3];
    
}

//------------------------------------------------------------------------------
