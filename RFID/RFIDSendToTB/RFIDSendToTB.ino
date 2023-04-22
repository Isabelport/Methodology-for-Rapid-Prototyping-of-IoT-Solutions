//Libraries
#include <SPI.h>          //https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>      //https://github.com/miguelbalboa/rfid
#include <WiFi.h>         // WiFi control for ESP32
#include <ThingsBoard.h>  // ThingsBoard SDK
#include "secrets.h"
//Constants
#define SS_PIN 5
#define RST_PIN 0
//Parameters
//const int ipaddress[4] = { 103, 97, 67, 25 };
//const int task[num_task][4] = { { 131, 126, 178, 203 },  //joining cables
//                                   { 132, 126, 178, 203 },  //screwing screws
//                                   { 133, 126, 178, 203 },
//                                   { 134, 126, 178, 203 } };
//Variables
byte nuidPICC[4] = { 0, 0, 0, 0 };
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

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

void setup() {
  //Init Serial USB
  Serial.begin(115200);
  Serial.println(F("Initialize System"));
  //init rfid D8,D5,D6,D7
  SPI.begin();
  rfid.PCD_Init();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();

  //initialize wifi
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();
}

void loop() {
  readRFID();
  delay(100);
}

void sendInfo(int task) {
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
  Serial.println("Sending data...");
  const int data_items = 1;

  Telemetry data[data_items] = {
    { "task", task },
  };
  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}

//returns hashtag of the rfid uuid
int hashFunc(byte *buffer, int size) {
  String bufferStr = "";
  int bufferAll;
  int rest;


  for (int i = 0; i < size; i++) {
    bufferStr = bufferStr + String(buffer[i]);
  }

  bufferAll = bufferStr.toInt(); //convert to one int
  rest = bufferAll % 17;

  return rest;
}

//dump a byte array as dec values to Serial.
  void printDec(byte * buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], DEC);
    }
  }

void readRFID(void) { /* function readRFID */
  int task;
  ////Read RFID card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new 1 cards
  if (!rfid.PICC_IsNewCardPresent())
    return;
  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;
  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  Serial.print(F("RFID In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  task = hashFunc(rfid.uid.uidByte,  rfid.uid.size);

  //If task found, send info to TB
  Serial.println();
  Serial.print("Task being performed: ");
  Serial.println(task);
  sendInfo(task);

  Serial.println();
  

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}