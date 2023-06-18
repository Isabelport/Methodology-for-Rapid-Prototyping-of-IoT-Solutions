#include <WiFi.h>           // WiFi control for ESP32
#include <ThingsBoard.h>    // ThingsBoard SDK
#include "secrets.h"


// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

#define SERIAL_DEBUG_BAUD    115200
#define YES 1
#define NO 0

const int sensor =  34;    
float prev_val = 0.0;
int sendUpdate = -1;


void InitWiFi(){
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  WiFi.begin(STA_SSID, STA_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("Connected to AP");
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

void sendInfo(float val){
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500); //return;
  }
  // Reconnect to ThingsBoard, if needed
  while (!tb.connected()) {
    Serial.println("Connecting to The ThingsBoard");
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      delay(500); //return;
    }
  }
  Serial.println("Sending data...");
  const int data_items = 1;

  //if(state==1){
  //Serial.println("No obstacle detected");
  //Serial.print(state);
  //}
  //else{
  //Serial.println("Obstacle detected");
  //}

  Telemetry data[data_items] = {
    { "voltage", val },
  }; 
    tb.sendTelemetry(data, data_items);
  
  // Process messages
  tb.loop();
}

int newVal(float val){

  if (abs(val - prev_val) <= 0.2){
    //prev_val = val;
    return NO;
  }
  else{
    prev_val = val;
    return YES;
  }
}

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  //pinMode(sensor, INPUT);

  //initialize wifi
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();

}

void loop() {

  int readVal = analogRead(sensor);
  float sensorVal = readVal * (3.3 / 4095.0);
  Serial.println(sensorVal);
  //sendUpdate = 1 if new state 
  sendUpdate = newVal(sensorVal);
  //Send to TB new information
  if(sendUpdate){
    sendInfo(sensorVal);
  }
  delay(100);
}