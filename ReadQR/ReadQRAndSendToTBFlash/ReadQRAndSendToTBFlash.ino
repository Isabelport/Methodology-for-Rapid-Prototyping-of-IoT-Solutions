#include <Arduino.h>
#include <ESP32QRCodeReader.h>
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
#define LED_BUILTIN 4
int last_id = 0;
unsigned long time_ = 0;

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);


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

void sendInfo(char idx){
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500); //return;
  }
  // Reconnect to ThingsBoard, if needed
  while (!tb.connected()) {
    Serial.println("Connecting TB");
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      delay(500); //return;
    }
  }
  Serial.println("Send");
  const int data_items = 1;
  //Serial.print("Passing carrier: ");
  //Serial.println(idx, DEC);
  Telemetry data[data_items] = {
    { "id", idx },
  };

    tb.sendTelemetry(data, data_items);
  
  // Process messages
  tb.loop();
}

int newCarrier(int id, unsigned long curr){

  //Serial.print("Not seen in: ");
  //Serial.print(millis()-curr);
  //Serial.println(" ms");


  if (id == last_id){
    if (millis() - curr > 5000){
      //Serial.println ("Too long since last seen");    
      return YES;
    }
    //Serial.println("Carrier already seen");
    return NO;
  }
  else{
    //Serial.println("New carrier");
    last_id = id;
    return YES;
  }
}

int conc_int(int a, int b){ // ex a=3 b =2
  a = a - '0';
  b = b - '0';
  if (a == 0){
    return b;
  }
  else {
    return (a*10 + b);  //3*10 + 2 = 32
  }
}

//Basic QR Code function
void onQrCodeTask(void *pvParameters)
{
  struct QRCodeData qrCodeData;
  while (true)
  {
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      //Serial.println("Found QRCode");
      digitalWrite(LED_BUILTIN, HIGH);
      //Serial.println("Flash on");
      delay(2000);
      if (qrCodeData.valid)
      {
        char * id = 0;
        int idi = 0;
        int sendUpdate = 0;

        id = (char*)qrCodeData.payload; 
        idi = conc_int((int)(*id), (int)*(id+1)); 
        //Serial.print("Carrier id: ");
        Serial.println(idi);
        
        //sendUpdate = 1 if new carrier is found
        sendUpdate = newCarrier(idi, time_);
        //Send to TB new information
        if(sendUpdate){
          time_ = millis();
          sendInfo(idi);
          digitalWrite(LED_BUILTIN, LOW);
          delay(200);
          digitalWrite(LED_BUILTIN, HIGH);
          delay(200);
          digitalWrite(LED_BUILTIN, LOW);
          delay(200);
          digitalWrite(LED_BUILTIN, HIGH);
          delay(200);
          digitalWrite(LED_BUILTIN, LOW);
          
        }
      }
      else
      {
        Serial.print("Inv ");
        //Serial.println((const int)*qrCodeData.payload);
        digitalWrite(LED_BUILTIN, LOW);
        
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


void setup()
{
  Serial.begin(SERIAL_DEBUG_BAUD);
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);
  
  //initialize wifi
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();

  reader.setup();
  Serial.println("Setup QRCode Reader");
  reader.beginOnCore(1);
  Serial.println("Begin QR Code reader");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);  
  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);

}

void loop()
{

  delay(100);
}