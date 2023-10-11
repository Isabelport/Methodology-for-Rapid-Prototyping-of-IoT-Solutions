#include <WiFi.h>         // WiFi control for ESP32
#include <ThingsBoard.h>  // ThingsBoard SDK
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "secrets.h"

#define implementation

//program vars
#define AWAY 0
#define NEAR 1
#define MAX_DEVICES 15
#define MAX_TIME 15000        //15s
#define TIME_THRESHOLD 15050  //15.05s
#define RSSI_THRESHOLD 5      //was at 5

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

int scanTime = 5;  //In seconds was 10
BLEScan* pBLEScan;

int active[MAX_DEVICES][3];  //[0] - id ; [1] - rssi; [2] to send update ?
//unsigned long Times[MAX_DEVICES];
unsigned long Times[MAX_DEVICES];  //last seen in ms (from active)
int active_beacons = 0;
int flag_send = 0;
unsigned long last_timestamp;
int setup_;

/*
 * Finds index of active[] with beacon ID = id. 
 * Returns: 
 *  -> idx (index) if beacon already active (is present in active[]).
 *  -> -1 if not found (beacon not active yet). 
 */
int getIndex(int id) {
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (active[i][0] == id)
      return i;
  }
  return -1;
}

/*
 * Adds new beacon found to list of active beacons (active[]).
 * If no entry is free in active[], removes the beacon not seen the longest and sends info.
 * Returns: idx/to_remove - index of new device in active[].
 */
int addDevice(int id) {
  int idx = -1;
  int to_remove = -1;
  unsigned long oldest = 0;

  for (int i = 0; i < MAX_DEVICES; i++) {

    if (active[i][0] == -1) {
      idx = i;
      break;
    } else if (active_beacons == MAX_DEVICES && (Times[i] < oldest || oldest == 0)) {
      to_remove = i;
      oldest = Times[i];
      Serial.println("Max beacons -> removing oldest");  //debug
    }
  }

  if (idx != -1) {
    active[idx][0] = id;
    return idx;
  } else {
    sendInfo(AWAY, to_remove);
    active[to_remove][0] = id;
    active_beacons--;
    return to_remove;
  }
}

/*
 * Updates rssi and time of entry idx of active[].
 */
void updateDevice(int idx, int new_rssi, unsigned long new_time) {
  active[idx][1] = new_rssi;
  Times[idx] = new_time;
}

/*
 * Sends new information to server.
 * If type is NEAR - New beacon has been discovered.
 * If type is AWAY - A known beacon has not been seen in a while.
 */
void sendInfo(int type, int idx) {
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
  Serial.println("-> Sending data...");
  const int data_items = 2;
  if (type == NEAR) {
    //device active[idx][0] is close with rssi active[idx][1]
    Serial.print("Beacon NEAR : ");
    Serial.print(active[idx][0], HEX);
    Serial.print(",  RSSI: ");
    Serial.println(active[idx][1]);
    Telemetry data[data_items] = {
      //{ "state", NEAR },
      { "id", active[idx][0] },
      { "rssi", active[idx][1] },
    };
    tb.sendTelemetry(data, data_items);
  } else if (type == AWAY) {
    if (setup_ == 1) {
      Serial.print("Initializing Beacon : ");
      Serial.println(idx);
      Telemetry data[data_items] = {
        //{ "state", AWAY },
        { "id", idx },
        { "rssi", -100 },
      };
      tb.sendTelemetry(data, data_items);
    } else {
      //device active[idx][0] has not been seen with rssi active[idx][1]
      Serial.print("Beacon OUT : ");
      Serial.println(active[idx][0], HEX);
      Telemetry data[data_items] = {
        //{ "state", AWAY },
        { "id", active[idx][0] },
        { "rssi", -100 },
      };
      tb.sendTelemetry(data, data_items);
    }
  }
  // Process messages
  tb.loop();
}

/*
 * Checks if beacons in active[] have not been seen in more than time_THRESHOLD.
 * If so, removes beacon from active[] and informs server.
 */

void checkState() {
  Serial.println("--- checkState() ---");

  //Serial.println(last_timestamp);
  for (int i = 0; i < MAX_DEVICES; i++) {
    /*
    if (active[i][0] != -1) {
      Serial.print("Active");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(active[i][0]);
      Serial.print(";");
      Serial.print(active[i][1]);
      Serial.print(";");
      Serial.println(active[i][2]);
      Serial.print("Times");
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(Times[i]);
      Serial.println(last_timestamp);
    }*/
    if (Times[i] != 0 && (last_timestamp - Times[i]) > TIME_THRESHOLD) {
      Serial.print("Removing Beacon: ");
      Serial.println(active[i][0]);
      sendInfo(AWAY, i);
      active_beacons--;

      active[i][0] = -1;  //id
      active[i][1] = 0;   //rssi
      Times[i] = (unsigned long)0;
    }
  }
}

/* Sends beacon information if during scan: 
 *  - new beacon was seen
 *  - rssi of a beacon changed considerably */
void sendUpdate() {
  Serial.println("--- sendUpdate() ---");
  for (int i = 0; i < MAX_DEVICES; i++) {
    // if beacon info was updated
    if (active[i][2] == 1) {
      sendInfo(NEAR, i);
      active[i][2] = 0;
    }
  }
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    uint8_t* advertisement = (uint8_t*)advertisedDevice.getManufacturerData().data();
    if (advertisedDevice.getManufacturerData().length() > 22) {
      if (memcmp((uint8_t*)uuid_system, (uint8_t*)&advertisement[4], 16) == 0) {
        int beaconRSSI = advertisedDevice.getRSSI();
        int beaconID = advertisement[21];  
        Serial.print("Found! Beacon ID: ");
        Serial.println(beaconID, HEX);
        // if beacon seen last scan, get its index in active[], else idx=-1 (to add it to active[])
        int idx = getIndex(beaconID);
        if (idx == -1) {
          idx = addDevice(beaconID);
          Serial.println("update Times[] and active[]");
          updateDevice(idx, beaconRSSI, millis());  // update Times[] and active[]
          active[idx][2] = 1;
          flag_send = 1;
          active_beacons++;
        } else if (abs(abs(beaconRSSI) - abs(active[idx][1])) > RSSI_THRESHOLD) {
          updateDevice(idx, beaconRSSI, millis());  // rssi changed significantly
          Serial.println("rssi changed significantly");
          active[idx][2] = 1;
          flag_send = 1;
        } else {
          updateDevice(idx, active[idx][1], millis());  //update just time
        }
        Serial.print("RSSI: ");
        Serial.println(beaconRSSI);
        Serial.println();
      }
    }
  }
};

// Setup an application
void setup() {
#ifndef implementation
  Serial.begin(115200);
#endif
  //initialize wifi
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();
  //initialize ble
  btStart();
  InitBLE();
  //initialize timestamp
  last_timestamp = millis();
  setup_ = 1;

  for (int i = 0; i < MAX_DEVICES; i++) {
    sendInfo(AWAY, i + 1);  //initializes all beacons in ThingsBoard to Away
    //initialize devices array when receiver is turned on.
    active[i][0] = -1;
    active[i][1] = 0;
    active[i][2] = 0;
    Times[i] = 0;
    /*
    Serial.print("id: ");
    Serial.print(active[i][0]);
    Serial.print("rssi: ");
    Serial.print(active[i][1]);
    Serial.print("to send: ");
    Serial.println(active[i][2]);*/
  }
  setup_ = 0;
}

// Main application loop
void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  //Serial.println("Scan done!");
  pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
  for (int i = 0; i < MAX_DEVICES; i++) {
  }
  if (flag_send) {
    sendUpdate();
    flag_send = 0;
  }

  if (millis() - last_timestamp > MAX_TIME) {
    last_timestamp = millis();
    Serial.print("Active beacons: ");
    Serial.println(active_beacons);
    if (active_beacons > 0)
      checkState();
  }
  delay(1000);
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

void InitBLE() {
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  Serial.println("BLE initialized");
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
