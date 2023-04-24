/*
  Communicate with BME280s with different I2C addresses
  Nathan Seidle @ SparkFun Electronics
  March 23, 2015

  Modified by Donovan Schlekat @ UNC Chapel Hill for PHYS 231
  April 23, 2023

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14348 - Qwiic Combo Board
  https://www.sparkfun.com/products/13676 - BME280 Breakout Board

  This example shows how to connect two sensors on the same I2C bus.

  The BME280 has two I2C addresses: 0x77 (jumper open) or 0x76 (jumper closed)

  Hardware connections:
  BME280 -> I2C Multiplexer -> Arduino
  GND -> edit -> GND
  3.3 -> edit -> 3.3
  SDA -> edit -> A4
  SCL -> edit -> A5
*/

/*
Sensor detection and communication
*/

#include <Wire.h>

#include "SparkFunBME280.h"
BME280 mySensorA; //Uses default I2C address 0x77
BME280 mySensorB; //Uses I2C address 0x76 (jumper closed)

//Variables for heartbeat routine
int i = 0;
int grnLEDPin = 12;

//Connection bools
bool connectionA = 1;
bool connectionB = 1;

/*
Wireless Connection and Blynk Communication - Core Lab 8
Created by Rui Santos, modified by Dr. J, modified by Donovan Schlekat
*/

// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "22-23-UNC-PSK";
const char* password = "ReachHighAndDreamBig";
const char *yourname = "Donovan Schlekat"; // don't show a video with Dr J's name in it

char web_server_title[100];

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliary variables to store the current output state
String ESP12_LED_State = "off";
String nodeMCU_LED_State = "off";

// Assign output variables to GPIO pins
const int ESP12_LED = 2;    //this is the LED by the antenna
const int nodeMCU_LED = 16; //this is the LED by the USB connector

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// reversed source/sink logic
// you should probably ask about this on Discord
#define lo HIGH
#define hi LOW

void setup()
{
  pinMode(grnLEDPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("Connecting...");

  Wire.begin();

  mySensorA.setI2CAddress(0x77); //The default for the SparkFun Environmental Combo board is 0x77 (jumper open).
  //If you close the jumper it is 0x76
  //The I2C address must be set before .begin() otherwise the cal values will fail to load.

  if(mySensorA.beginI2C() == false) {
    Serial.println("Sensor A connect failed");
    connectionA = 0;
  }

  mySensorB.setI2CAddress(0x76); //Connect to a second sensor

  if (mySensorB.beginI2C() == false) {
    Serial.println("Sensor B connect failed");
    connectionB = 0;
  }


  // mySensorA.settings.commInterface = I2C_MODE;
	// mySensorA.settings.runMode = 3; //  3, Normal mode
	// mySensorA.settings.tStandby = 0; //  0, 0.5ms
	// mySensorA.settings.filter = 0; //  0, filter off
	// //tempOverSample can be:
	// //  0, skipped
	// //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	// mySensorA.settings.tempOverSample = 1;
	// //pressOverSample can be:
	// //  0, skipped
	// //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  //   mySensorA.settings.pressOverSample = 1;
	// //humidOverSample can be:
	// //  0, skipped
	// //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	// mySensorA.settings.humidOverSample = 1;

  // mySensorB.settings.commInterface = I2C_MODE;
	// mySensorB.settings.runMode = 3; //  3, Normal mode
	// mySensorB.settings.tStandby = 0; //  0, 0.5ms
	// mySensorB.settings.filter = 0; //  0, filter off
	// //tempOverSample can be:
	// //  0, skipped
	// //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	// mySensorB.settings.tempOverSample = 1;
	// //pressOverSample can be:
	// //  0, skipped
	// //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  //   mySensorB.settings.pressOverSample = 1;
	// //humidOverSample can be:
	// //  0, skipped
	// //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
	// mySensorB.settings.humidOverSample = 1;
}

void loop()
{
  //Display readings from each connection
  if (connectionA == 1) {
    Serial.print("HumidityA: ");
    Serial.print(mySensorA.readFloatHumidity(), 0);

    Serial.print("; PressureA: ");
    Serial.print(mySensorA.readFloatPressure(), 0);

    Serial.print("; TempA: ");
    //Serial.print(mySensorA.readTempC(), 2);
    Serial.print(mySensorA.readTempF(), 2);

    Serial.println();
  }

  if (connectionB == 1) {
    Serial.print("; HumidityB: ");
    Serial.print(mySensorB.readFloatHumidity(), 0);

    Serial.print("; PressureB: ");
    Serial.print(mySensorB.readFloatPressure(), 0);

    Serial.print("; TempB: ");
    //Serial.print(mySensorB.readTempC(), 2);
    Serial.print(mySensorB.readTempF(), 2);

    Serial.println();
  }


  //Heartbeat routine
  if (i % 2 == 1) {
    digitalWrite(grnLEDPin, HIGH);
    delay(1000);
  }
  else {
    digitalWrite(grnLEDPin, LOW);
    delay(1000);
  }

  //Loop for heartbeat routine
  i ++ ;
  if (i == 16) {
    i = 0;
  }
}

// void scanner ()
// {
//    byte error, address;
//    int nDevices;
//  Serial.println("Scanning…");
//  nDevices = 0;
//    for (address = 1; address < 127; address++ )
//    {
//      // The i2c_scanner uses the return value of
//      // the Write.endTransmisstion to see if
//      // a device did acknowledge to the address.
//      Wire.beginTransmission(address);
//      error = Wire.endTransmission();
//  if (error == 0) {   Serial.print("I2C device found at address 0x");   if (address < 16)     Serial.print("0");   Serial.print(address, HEX);   Serial.println("  !");   nDevices++; } else if (error == 4) {   Serial.print("Unknown error at address 0x");   if (address < 16)     Serial.print("0");   Serial.println(address, HEX); }
//  }
//    if (nDevices == 0)
//      Serial.println("No I2C devices found");
//    else
//      Serial.println("done");
//  delay(5000);           // wait 5 seconds for next scan
//  }

