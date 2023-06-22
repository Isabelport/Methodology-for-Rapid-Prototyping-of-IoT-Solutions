/*
  Reading distance from the laser based VL53L1X
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 4th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/14667

  This example prints the distance to an object.

  Are you getting weird readings? Be sure the vacuum tape has been removed from the sensor.
*/

#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

//Optional interrupt and shutdown pins.
#define INTERRUPT_PIN 18
#define SHUTDOWN_PIN 17
#define SCL_PIN 21
#define SDA_PIN 16

#define DIST_CHANGE 150         //mm ->15cm
#define MIN_TIME 30             //s
#define DISTANCE_THRESHOLD 800  //mm -> 80cm

unsigned long last_send = 0;
int last_distance = 0;

//SFEVL53L1X distanceSensor;
//Uncomment the following line to use the optional shutdown and interrupt pins.
SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

void setup()
{

  delay(3000);
  Serial.println("yo");
  Wire.setPins(SDA_PIN, SCL_PIN);
  Serial.begin(115200);
  Serial.println("VL53L1X Qwiic Test");

  //shutdown is active at LOW
  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  Serial.println("hello");
  //i2c_scan();
  initDistSensor();
}

void i2c_scan() {
  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of the Write.endTransmisstion
    // to see if a device did acknowledge to the address.
    Wire.beginTransmission(address);

    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  //delay(2000);  // wait 5 seconds for next scan
}

void initDistSensor() {
  Serial.println("hello2");
  while (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    delay(500);
  }
  Serial.println("Sensor online!");
  /*
  distanceSensor.setDistanceModeLong();  
  distanceSensor.setTimingBudgetInMs(103);  //test with 50, 140,200
  distanceSensor.setIntermeasurementPeriod(1000); //was 8000

  distanceSensor.startRanging();
*/                
}

void loop(void)
{
  /*
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady())
  {
    delay(1);
  }
  */
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor

  Serial.print("Distance(mm): ");
  Serial.print(distance);
  Serial.println();
  /*
  int to_send = checkDistance(distance);
  if (to_send){
    Serial.print("Sending distance:"); Serial.println(distance);
    last_send = millis();
    last_distance = distance;
  }
  */
}

//tells if the new distance is sent to TB or not, if it has 
//been longer than MIN_TIME or it changed more than DIST_CHANGE
bool checkDistance(int distance){

  if((millis() - last_send > MIN_TIME) or (abs(distance - last_distance)>DIST_CHANGE)){
    return 1;
  }
  else 
    return 0 ;
  
  
}









