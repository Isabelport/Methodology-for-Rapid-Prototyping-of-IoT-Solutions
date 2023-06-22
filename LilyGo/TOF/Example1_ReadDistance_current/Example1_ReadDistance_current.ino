#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

//Optional interrupt and shutdown pins.
// branco - gnd, preto - sda, vermelho - scl, verde - interrupt2, amarelo - interrupt1, azul - 5V
/*
#define INTERRUPT_PIN 43
#define SHUTDOWN_PIN 44
#define SCL_PIN 18
#define SDA_PIN 17
*/
#define INTERRUPT_PIN 18
#define SHUTDOWN_PIN 17
#define SCL_PIN 21
#define SDA_PIN 16

SFEVL53L1X distanceSensor;
//Uncomment the following line to use the optional shutdown and interrupt pins.
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

void setup(void)
{
    //pinMode(SDA, INPUT_PULLUP);
  //pinMode(SCL, INPUT_PULLUP);
  delay(4000);

  Wire.begin(SDA_PIN, SCL_PIN); //SDA SCL
  Serial.begin(115200);

  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH);
  //pinMode(INTERRUPT_PIN, OUTPUT);
  //digitalWrite(INTERRUPT_PIN, HIGH);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);

  Serial.println("VL53L1X Qwiic Test");
  

  if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println("Sensor online!");
}

void loop(void)
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
