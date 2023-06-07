#include <Wire.h>
#include <WiFi.h>           // WiFi control for ESP32
#include <ThingsBoard.h>    // ThingsBoard SDK
#include "time.h"

#define TRUE 1
#define FALSE 0

const char* ntpServer = "pool.ntp.org"; // NTP server to request epoch time
unsigned long epochTime; // Variable to save current epoch time

#define MEDIR_CAP 3  //PA09
#define CAP_V A2
#define EN_SENSOR 4
const byte DIO1 = A5;
const byte int_tof = 13;        //PA20
const byte XSHUT = 6;           //PA19
// branco - gnd, preto - sda, vermelho - scl, verde - interrupt2, amarelo - interrupt1, azul - 5V

//VL DEFS
#include "ComponentObject.h"
#include "RangeSensor.h"
#include "SparkFun_VL53L1X.h"
#include "vl53l1x_class.h"
#include "vl53l1_error_codes.h"

#define DIST_CHANGE 150         //mm ->15cm
#define MIN_TIME 30             //s
#define DISTANCE_THRESHOLD 800  //mm -> 80cm

volatile bool int_flag = false;
SFEVL53L1X distanceSensor;

#include "secrets.h"
// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

unsigned long now = 0;

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(STA_SSID, STA_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}

void sendInfo(int dist) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  while (!tb.connected()) {
    Serial.println("Connecting to The ThingsBoard");
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      delay(500);  //return;
    }
  }
  const int data_items = 1;
  Telemetry data[data_items] = {
    { "distance", dist },
  };

  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}


void i2c_scan() {
  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of the Write.endTransmisstion
    // to see if a device did acknowledge to the address.
    Serial.println("Scanning before begin ...");
    Wire.beginTransmission(address);
    Serial.println("Scanning after begin ...");

    error = Wire.endTransmission();
    Serial.println("Scanning after end trans ...");
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

  delay(5000);  // wait 5 seconds for next scan
}

void initDistSensor() {

  //sodaq_wdt_safe_delay(1000);
  //Serial.println("After Wire Begin!");

  float distance = 0;
  //Begin returns 0 on a good init
  if (distanceSensor.begin() == 0) {
    //Serial.println("Sensor online!");
    //sodaq_wdt_safe_delay(100);

    distanceSensor.setDistanceModeLong();  //set long distance mode, was short before

    distanceSensor.setTimingBudgetInMs(103);  //test with 50, 140,200

    //sodaq_wdt_safe_delay(100);

    distanceSensor.setInterruptPolarityLow();

    //sodaq_wdt_safe_delay(100);

    distanceSensor.setIntermeasurementPeriod(8000);

    distanceSensor.setDistanceThreshold(DISTANCE_THRESHOLD, 1800, 0);

    Serial.println("THRESHOLDS: window,low,high");
    Serial.println(distanceSensor.getDistanceThresholdWindow());
    Serial.println(distanceSensor.getDistanceThresholdLow());
    Serial.println(distanceSensor.getDistanceThresholdHigh());


    distanceSensor.startRanging();
    distanceSensor.clearInterrupt();

    //sodaq_wdt_safe_delay(500);
    Serial.println("sensor initialized");
  } else
    Serial.println("unable to init sensor");
}

void interrupt() {
  int_flag = true;
}


void InitWiFi(){
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  WiFi.begin(STA_SSID, STA_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void setup() {
  asm(".global _printf_float");  //very impotant for sprintf with float
  Serial.begin(115200);
  // Bootup sodaq_wdt_safe_delay to programm the board.
  //setup_BOD33();
  //nvm_wait_states();

  //initialize wifi
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();

  //Wire.begin(I2C_SDA, I2C_SCL);
  Wire.begin();
  //sodaq_wdt_safe_delay(5000);
  //sodaq_wdt_enable(WDT_PERIOD_8X);  //ENABLE WDT
  //setup_rtc_debug();

  pinMode(MEDIR_CAP, OUTPUT);
  digitalWrite(MEDIR_CAP, LOW);
  pinMode(CAP_V, INPUT);
  //analogReference(AR_DEFAULT);

  pinMode(XSHUT, OUTPUT);
  digitalWrite(XSHUT, HIGH);

  pinMode(DIO1, INPUT);

  initDistSensor();

  pinMode(int_tof, INPUT_PULLUP);
  //pinMode(int_tof, INPUT);

  attachInterrupt(int_tof, interrupt, FALLING);  //Pin of interrupt is digital PIN2 AQUI PIN DE INTERRUPT
  //stuff to allow falling interrupt
  // Set the XOSC32K to run in standby
  //SYSCTRL->XOSC32K.bit.RUNSTDBY = 1;

  // Configure EIC to use GCLK1 which uses XOSC32K
  // This has to be done after the first call to attachInterrupt()
  //GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCM_EIC) | GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_CLKEN;

  //sodaq_wdt_reset();

  distanceSensor.clearInterrupt();

  //sodaq_wdt_safe_delay(1000);

  distanceSensor.clearInterrupt();

  digitalWrite(EN_SENSOR, LOW);

}

int cnt = 0;
int state = 0;
long last_send = 0;

void loop() {

  if (int_flag) {
    //sodaq_wdt_reset();
    cnt++;

    if (cnt == 2) {
      if (state) { //state = 1 and cnt = 2
        now = getTime();
        if (now - last_send > MIN_TIME) {
          int distance;

          digitalWrite(EN_SENSOR, HIGH);

          distance = distanceSensor.getDistance();  //Get measurement distance.

          Serial.print("Distance(mm): ");
          Serial.println(distance);

          sendInfo(distance);
          
          now = getTime();
          last_send = now;

          //UPDATE INT
          if (distance < DISTANCE_THRESHOLD) {
            int min_value = distance - DIST_CHANGE;
            int max_value = distance + DIST_CHANGE;
            if (max_value > DISTANCE_THRESHOLD)
              max_value = DISTANCE_THRESHOLD;
            if (min_value < 20)
              min_value = 20;  //lets make min distance 2cm
            distanceSensor.stopRanging();
            distanceSensor.clearInterrupt();
            distanceSensor.setDistanceThreshold(min_value, max_value, 2);
            distanceSensor.startRanging();
            distanceSensor.clearInterrupt();
            Serial.println("INT updated !");
          } else {
            distanceSensor.stopRanging();
            distanceSensor.clearInterrupt();
            // to change window of interrupt from 2 to 0 initialization is needed
            initDistSensor();
            state = 0;
            Serial.println("INT updated && state = 0 !");
          }
            Serial.println("window,low,high: ");
            Serial.print(distanceSensor.getDistanceThresholdWindow());
            Serial.print(", ");
            Serial.print(distanceSensor.getDistanceThresholdLow());
            Serial.print(", ");
            Serial.println(distanceSensor.getDistanceThresholdHigh());
        }
      } 
      else { // state = 0 cnt = 2
        now = getTime();
        if (now - last_send > MIN_TIME) {
          int distance;
          digitalWrite(EN_SENSOR, HIGH);
          distance = distanceSensor.getDistance();  //Get measurement distance.

          Serial.print("Distance(mm): ");
          Serial.println(distance);

          sendInfo(distance);

          // UPDATE INT
          int min_value = distance - DIST_CHANGE;
          int max_value = distance + DIST_CHANGE;
          if (max_value > DISTANCE_THRESHOLD)
            max_value = DISTANCE_THRESHOLD;
          if (min_value < 20)
            min_value = 20;  //lets make min distance 2cm
          distanceSensor.stopRanging();
          distanceSensor.clearInterrupt();
          distanceSensor.setDistanceThreshold(min_value, max_value, 2);
          distanceSensor.startRanging();
          distanceSensor.clearInterrupt();

          state = 1;
          Serial.println("INT updated && state = 1 !");
          Serial.println("window,low,high: ");
          Serial.print(distanceSensor.getDistanceThresholdWindow());
          Serial.print(", ");
          Serial.print(distanceSensor.getDistanceThresholdLow());
          Serial.print(", ");
          Serial.println(distanceSensor.getDistanceThresholdHigh());
        
        }
      }
      cnt = 0;
    }

    int_flag = false;
    distanceSensor.clearInterrupt();

    digitalWrite(EN_SENSOR, LOW);

  }
}
