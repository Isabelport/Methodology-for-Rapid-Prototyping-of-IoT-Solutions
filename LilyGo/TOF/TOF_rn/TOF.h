#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

//Optional interrupt and shutdown pins.
#define INTERRUPT_PIN 18
#define SHUTDOWN_PIN 17
#define SCL_PIN 21
#define SDA_PIN 16

#define DIST_CHANGE 150         //mm ->15cm
#define MIN_TIME 30000             //ms -> 30s
#define DISTANCE_THRESHOLD 800  //mm -> 80cm

#define NEAR 1
#define AWAY 0

unsigned long last_send = 0;
int last_distance = 0;
bool state = 0; //NEAR 1 AWAY 0

//SFEVL53L1X distanceSensor;
//Uncomment the following line to use the optional shutdown and interrupt pins.
SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);



