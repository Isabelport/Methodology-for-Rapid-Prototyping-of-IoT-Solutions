#include <WiFi.h> // WiFi control for ESP32
#include <ThingsBoard.h>  // ThingsBoard SDK

// Initialize ThingsBoard client
WiFiClient espclient; //espclient
// Initialize ThingsBoard instance
ThingsBoard tb(espclient); //espclient
// the Wifi radio's status
int status = WL_IDLE_STATUS;
int prev_task = 0;
int count = 0;

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

void sendInfo_task(int sec, int task, int emp_id) {
  //first task sent is 0
  /*
  if (task == prev_task)
    count ++;
  else {
    prev_task = task;
    count = 1;
  }
  Serial.print("attention pickpocket "); Serial.println(count);*/

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
    //{ "min", min },
    { "empid", emp_id},
    { "sec", sec },
    { "task", task },
    };
  Serial.print("Sending info... min: "); Serial.print(sec/60); Serial.print(" sec: "); Serial.print(sec%60); Serial.print(" task: "); 
  Serial.println(task); Serial.print(" emp: "); Serial.println(emp_id);

  tb.sendTelemetry(datat, data_items);

  // Process messages
  tb.loop();
}

void sendInfo_final_task(int task_id, int av, int pr, int total_min, int emp_id) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();
  const int data_items1 = 3;
  //taskid indicates task related to this data
  Telemetry datat[data_items1] = {
    { "av", av },
    { "pr", pr },
    { "taskid", task_id }, 
    };
  // total seconds?
  const int data_items2 = 3;
    //task = 0 to show on things board that we are in break
  Telemetry datatt[data_items2] = {
    { "totalm", total_min },
    { "emp_id", emp_id },
    { "task", 0 }, 
    //{ "count", count },
    };
  Serial.print("Sending final info... av: "); Serial.print(av); Serial.print(" pr: "); Serial.print(pr); Serial.print(" taskid: "); Serial.print(task_id);  Serial.print(" total h: "); 
  Serial.print(total_min/60); Serial.print(" total m: "); Serial.println(total_min%60); Serial.print(" emp: "); Serial.println(emp_id); Serial.print(" task: "); Serial.println(0);
  //Serial.print(" break: ");
  tb.sendTelemetry(datat, data_items1);
  tb.sendTelemetry(datatt, data_items2);


  // Process messages
  tb.loop();
}
void sendInfo_tof(int dist, int stat, int emp_id) {
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
    //{ "emp_id", emp_id},
    };
  Serial.print("Sending info... dist: "); Serial.print(dist); Serial.print(" stat: "); Serial.print(stat); 
  Serial.print(" emp: "); Serial.println(emp_id);

  tb.sendTelemetry(datat, data_items);

  // Process messages
  tb.loop();
}
