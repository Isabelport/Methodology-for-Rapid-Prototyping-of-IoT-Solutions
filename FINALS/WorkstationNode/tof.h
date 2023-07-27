#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X
#include <Wire.h>

//Optional interrupt and shutdown pins.
#define INTERRUPT_PIN 18
#define SHUTDOWN_PIN 17
#define SCL_PIN 21
#define SDA_PIN 16

#define DIST_CHANGE 150         //mm ->15cm
#define MAX_TIME 60000             //ms -> 60s
#define MIN_TIME 15000             //ms -> 20s
#define DISTANCE_THRESHOLD 800  //mm -> 80cm

#define NEAR 1
#define AWAY 0

unsigned long last_send = 0;
int last_distance = 0;
bool state = 0; //NEAR 1 AWAY 0º

//SFEVL53L1X distanceSensor;
//Uncomment the following line to use the optional shutdown and interrupt pins.
SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

void initDistSensor() {
  if (distanceSensor.begin() != 0){  //Begin returns 0 on a good init
    Serial.println("ToF failed to begin. Please check wiring. Freezing...");
    while(distanceSensor.begin()){
      Serial.println("ToF failed to begin. Please check wiring. Freezing...");
      tft.drawString("Verificar ToF", 5, 80, 4);
      delay(500);
    }
    Serial.println("ToF ready!");
    tft.fillRect(10, 80, 40, 100, TFT_BLACK);
    tft.drawString("ToF conectado", 5, 80, 4);
    delay(1500);

  }
  else{
    Serial.println("ToF ready!");
    tft.drawString("ToF conectado", 5, 80, 4);
    delay(2000);
  }
  distanceSensor.setDistanceModeLong();  
  distanceSensor.setIntermeasurementPeriod(1000); //was 8000
  
  //distanceSensor.setTimingBudgetInMs(103);  //test with 50, 140,200
  
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  return;
}


int getDistance(){
  //distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  if (!distanceSensor.checkForDataReady()){
    return -1; //data not ready
  }
  
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  //distanceSensor.stopRanging();
  //Serial.print("Distance(mm): ");
  //Serial.print(distance);
  //Serial.println();

  return distance;
  
}

//tells if the new distance is sent to TB or not, if it has 
//been longer than MAX_TIME or it changed more than DIST_CHANGE
bool checkDistance(int distance){

  if( !(millis() - last_send < MIN_TIME) and 
    ((millis() - last_send > MAX_TIME) or (abs(distance - last_distance) > DIST_CHANGE))){
    return 1;
  }
  else 
    return 0 ;
  
  
}

void checkAndSendDistance(){
    int distance = getDistance();
    if (distance != -1) {
      int to_send = checkDistance(distance);
      if (to_send) {
        last_send = millis();
        last_distance = distance;
        if (distance <= DISTANCE_THRESHOLD) {
          state = 1;
        } else {
          state = 0;
        }
        Serial.print("Sending distance:");
        Serial.println(distance);
        sendInfo_tof(distance, state, emp_id);
      }
    }
}

