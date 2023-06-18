#include <WiFi.h> // WiFi control for ESP32
#include <ThingsBoard.h>  // ThingsBoard SDK


// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if (status != WL_CONNECTED) {
    WiFi.begin(STA_SSID, STA_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}

void InitWiFi() {
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  WiFi.begin(STA_SSID, STA_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void connectTB(){
  // Reconnect to ThingsBoard, if needed
  while (!tb.connected()) {
    Serial.println("Connecting to The ThingsBoard");
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      delay(500);  //return;
    }
  }
  Serial.println("Connected to ThingsBoard");
}

void sendInfo(int min, int sec, int task) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();
  //Serial.println("-> Sending data...");
  const int data_items = 3;
  Telemetry data[data_items] = {
    { "Minutes", min },
    { "Seconds", sec },
    { "task", task },
    };

  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}

void sendInfo_final(int av, int pr, int total_h, int total_min) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();

  const int data_items = 4;
  Telemetry data[data_items] = {
    { "Average", av },
    { "Personal Record", pr },
    { "total_h", total_h },
    { "total_min", total_min },
    };

  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}