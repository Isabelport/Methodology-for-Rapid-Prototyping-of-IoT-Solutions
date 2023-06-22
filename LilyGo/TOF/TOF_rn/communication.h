#include <WiFi.h> // WiFi control for ESP32
#include <ThingsBoard.h>  // ThingsBoard SDK


// Initialize ThingsBoard client
WiFiClient espclient; //espclient
// Initialize ThingsBoard instance
ThingsBoard tb(espclient); //espclient
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


void sendInfo_TASK(int min, int sec, int task) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();
  const int data_items = 3;
  Telemetry datat[data_items] = {
    { "min", min },
    { "sec", sec },
    { "task", task },
    };
  Serial.print("Sending info... min: "); Serial.print(min); Serial.print(" sec: "); Serial.print(sec); Serial.print(" task: "); Serial.println(task);

  tb.sendTelemetry(datat, data_items);

  // Process messages
  tb.loop();
}

void sendInfo_final_TASK(int task_id, int av, int pr, int total_h, int total_min) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();
  const int data_items1 = 3;
  //task = 0 to show on things board that we are in break
  Telemetry datat[data_items1] = {
    { "av", av },
    { "pr", pr },
    { "taskid", task_id }, 
    
    };
  // total seconds?
  // { "totals", 43 },
  const int data_items2 = 3;
  //taskid indicates task related to this data
  Telemetry datatt[data_items2] = {
    { "totalh", total_h },
    { "totalm", total_min },
    { "task", 0 }, 
    };
  Serial.print("Sending final info... av: "); Serial.print(av); Serial.print(" pr: "); Serial.print(pr); Serial.print(" taskid: "); Serial.print(task_id);  Serial.print(" total h: "); 
  Serial.print(total_h); Serial.print(" total m: "); Serial.print(total_min); Serial.print(" task: "); Serial.println("0");
  tb.sendTelemetry(datat, data_items1);
  tb.sendTelemetry(datatt, data_items2);


  // Process messages
  tb.loop();
}

void sendInfo_TOF(int dist, int stat) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();
  const int data_items = 2;
  Telemetry datat[data_items] = {
    { "distance", dist },
    { "stat", stat },
    };
  Serial.print("Sending info... dist: "); Serial.print(dist); Serial.print(" stat: "); Serial.println(stat);

  tb.sendTelemetry(datat, data_items);

  // Process messages
  tb.loop();
}