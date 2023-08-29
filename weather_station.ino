/*
  Written Donovan Schlekat @ UNC Chapel Hill for PHYS 231 / Argus Array Temperature Prototyping
  Github: https://github.com/dschlekat

  This sketch allows the user to connect multiple BME280 temperature sensors to a NodeMCU
  using a TCA9548A I2C multiplexer. The temperature, pressure and humidity readings from
  the sensors are then sent to the user's Blynk dashboard via a wireless connection, 
  where they can be viewed from anywhere in the world with an internet connection.

  Alternatively, readings can be displayed on a custom website local to the 
  network, or to a website posted online.

  Another option is to write the data via the NodeMCU's serial port to whatever
  method of data transmission is used by Argus.

  Hardware connections:

  The connections below assume one is connecting multiple temperature sensors to the I2C multiplexer.
  If one sensor is used per NodeMCU instead, connect the sensor to the microcontroller using the multiplexer connection schematic.
  
  +=====+
  BME280 -> I2C Multiplexer,NodeMCU
  GND -> Node GND          // Ground
  3.3 -> Node 3V3          // Power
  SDA -> Multiplexer SDn   // Communication
  SCL -> Multiplexer SCn   // Communication

  I2C Multiplexer -> NodeMCU
  GND -> GND   // Ground
  VIN -> 3V3   // Power
  SDA -> D2    // Communication
  SCL -> D1    // Communication
  +=====+
*/

/*
Sensor detection and communication
*/

#include <Wire.h>
#include "SparkFunBME280.h"

//Sets sensor names. Add additional sensors here.
BME280 SensorA; //Uses default I2C address 0x77
BME280 SensorB; //Uses I2C address 0x76 (jumper closed)

//Variables for heartbeat routine
int i = 0;
int ESP12_LED = 2;

//Connection bools. Add bools for additional sensors here.
bool connectionA = 1;
bool connectionB = 1;

/*
Wireless Connection and Blynk - One Option for Communication
*/

#define BLYNK_PRINT Serial

//Blynk template information. Accessed from Blynk's website.
#define BLYNK_TEMPLATE_ID "TMPL2WvMza-OA"
#define BLYNK_TEMPLATE_NAME "weather station"
#define BLYNK_AUTH_TOKEN "qZS43MEOj8SuF4Zq8ODpl7XZK8Bt2xku"

//Required libraries.
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// 
// Auth tocken accessed from Blynk App. Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials. Depends on network.
// Set password to "" for open networks.
char ssid[] = "23-24-UNC-PSK";
char pass[] = "HopeIsASuperpower";




// Multiplexer connection function. When connecting to a device, call the function with the number of the bus (1-8) the device is connecting to.
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
  Serial.print(bus);
}

void setup()
{
  /*
  Connect to web server
  */
  yield();
  Serial.begin(9600);
  Serial.println("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass);
  
  /*
  Connect to sensors
  */
  pinMode(ESP12_LED, OUTPUT);
  Serial.println("Connecting to sensors...");

  Wire.begin();

  SensorA.setI2CAddress(0x77); //The default for the SparkFun Environmental Combo board is 0x77 (jumper open).
  //If you close the jumper it is 0x76

  TCA9548A(2);
  if(!SensorA.beginI2C()) {
    Serial.println("Sensor A connection failed");
    connectionA = 0;
  }
  else {
    Serial.println("Sensor A connected");
  }

  SensorB.setI2CAddress(0x77); //Connect to a second sensor

  TCA9548A(6);
  if (SensorB.beginI2C() == false) {
    Serial.println("Sensor B connection failed");
    connectionB = 0;
  }
  else {
    Serial.println("Sensor B connected");
  }
}

void loop()
{
  Blynk.run();

  //Display readings from each connected sensor
  if (connectionA == 1) {
    //Read values from sensor
    //Note: Farenheit can be changed to Celsius
    float tempA = SensorA.readTempF();
    //float tempA = SensorA.readTempC();
    float humA = SensorA.readFloatHumidity();
    float pressA = SensorA.readFloatPressure() / 100;

    //Print to serial port (optional)
    Serial.print("HumidityA: ");
    Serial.print(humA, 0);
    Serial.print("%RH; PressureA: ");
    Serial.print(pressA, 0);
    Serial.print("hPa; TempA: ");
    Serial.print(tempA, 2);
    Serial.print("F");
    Serial.println();

    //Post data to Blynk
    Blynk.virtualWrite(V1, tempA);
    Blynk.virtualWrite(V2, pressA);
    Blynk.virtualWrite(V3, humA);
  }

  if (connectionB == 1) {
    //Read values from sensor
    //Note: Farenheit can be changed to Celsius
    float tempB = SensorB.readTempF();
    //float tempB = SensorB.readTempC();
    float humB = SensorB.readFloatHumidity();
    float pressB = SensorB.readFloatPressure() / 100;

    //Print to serial port (optional)
    Serial.print("HumidityB: ");
    Serial.print(humB, 0);
    Serial.print("%RH; PressureB: ");
    Serial.print(pressB, 0);
    Serial.print("hPa; TempB: ");
    Serial.print(tempB, 2);
    Serial.print("F");
    Serial.println();

    //Post data to Blynk
    Blynk.virtualWrite(V4, tempB);
    Blynk.virtualWrite(V5, pressB);
    Blynk.virtualWrite(V6, humB);
  }


  //Heartbeat routine
  if (i % 2 == 1) {
    digitalWrite(ESP12_LED, HIGH);
    delay(1000);
  }
  else {
    digitalWrite(ESP12_LED, LOW);
    delay(1000);
  }

  //Loop for heartbeat routine
  i ++ ;
  if (i == 2) {
    i = 0;
  }
}