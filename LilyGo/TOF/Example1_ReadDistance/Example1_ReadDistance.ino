#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

//Optional interrupt and shutdown pins.
//TOF constants
#define INTERRUPT_PIN 18
#define SHUTDOWN_PIN 17
#define SCL_PIN 21
#define SDA_PIN 16

#define DIST_CHANGE 150         //mm ->15cm
#define MIN_TIME 30             //s
#define DISTANCE_THRESHOLD 800  //mm -> 80cm

int distance;

//SFEVL53L1X distanceSensor();
//Uncomment the following line to use the optional shutdown and interrupt pins.
SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

void setup()
{
  Serial.begin(115200);
  delay(4000); 
  Serial.println("start");
  //shutdown e output, interrupt e input
  Wire.begin(SDA_PIN, SCL_PIN); //SDA SCL

  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);  

  Serial.println("VL53L1X Qwiic Test");

  if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println("Sensor online!");

  initDistSensor();

  //i2c_address();
}


void initDistSensor() {

  float distance = 0;
  //Begin returns 0 on a good init
  if (distanceSensor.begin() == 0) {
    distanceSensor.setDistanceModeLong();  //set long distance mode, was short before
    distanceSensor.setTimingBudgetInMs(103);  //test with 50, 140,200
    distanceSensor.setInterruptPolarityLow();
    distanceSensor.setIntermeasurementPeriod(8000);
    distanceSensor.setDistanceThreshold(DISTANCE_THRESHOLD, 1800, 0);

    Serial.println("THRESHOLDS: window,low,high");
    Serial.println(distanceSensor.getDistanceThresholdWindow());
    Serial.println(distanceSensor.getDistanceThresholdLow());
    Serial.println(distanceSensor.getDistanceThresholdHigh());

    distanceSensor.startRanging();
    distanceSensor.clearInterrupt();

    Serial.println("Sensor initialized");
  } else
    Serial.println("Unable to init sensor");
}

//detects if there is a change in the distance to the tof
void change_dist(int distance){
  if (distance < DISTANCE_THRESHOLD) {
    int min_value = distance - DIST_CHANGE;
    int max_value = distance + DIST_CHANGE;
    if (max_value > DISTANCE_THRESHOLD)
      max_value = DISTANCE_THRESHOLD;
    if (min_value < 20)
      min_value = 20;  //lets make min distance 2cm
  }

  Serial.println("window,low,high: ");
  Serial.print(distanceSensor.getDistanceThresholdWindow());
  Serial.print(", ");
  Serial.print(distanceSensor.getDistanceThresholdLow());
  Serial.print(", ");
  Serial.println(distanceSensor.getDistanceThresholdHigh());
  
  Serial.print("send distance: "); Serial.println("distance"); 
}

void loop()
{
  distance = readToF();
  change_dist(distance);

}

int readToF(){
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

  return distance;
}

void i2c_address() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}
