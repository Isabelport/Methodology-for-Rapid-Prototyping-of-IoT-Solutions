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
  Wire.begin();
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

void initDistSensor() {
  Serial.println("hello2");
  while (distanceSensor.begin() != 0){ //Begin returns 0 on a good init
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    delay(500);
  }
  Serial.println("Sensor online!");
           
}

void loop()
{
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady())
  {
    delay(1);
  }
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();
  
  Serial.print("Distance(mm): ");
  Serial.print(distance);
  Serial.println();

}










