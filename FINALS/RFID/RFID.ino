//Libraries
#include <SPI.h>          //https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>      //https://github.com/miguelbalboa/rfid
#include <WiFi.h>         // WiFi control for ESP32
#include <ThingsBoard.h>  // ThingsBoard SDK

#include "secrets.h"
//Constants
#define SS_PIN 5
#define RST_PIN 3
#define MISO_PIN 19
#define MOSI_PIN 23
#define SCK_PIN 18
//Variables
byte nuidPICC[4] = { 0, 0, 0, 0 };
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
unsigned long current = 0;
int lastSend = 0;

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

//gabarit is a working tray for the production line
const int num_of_gabarits = 24;
int rfid_gabarit[num_of_gabarits][4] = { { 225, 175, 204, 27 },  //1 NAOOO
                                         { 161, 197, 72, 72 },   //2 NAOOOOO
                                         { 192, 243, 214, 29 },  //3
                                         { 50, 220, 19, 76 },    //4
                                         { 162, 148, 223, 29 },  //5
                                         { 81, 41, 36, 71 },     //6
                                         { 163, 141, 244, 75 },  //7
                                         { 97, 63, 200, 69 },    //8
                                         { 146, 232, 98, 29 },   //9
                                         { 163, 180, 88, 50 },   //10
                                         { 242, 203, 81, 76 },   //11
                                         { 163, 163, 30, 77 },   //12
                                         { 147, 231, 204, 77 },  //13
                                         { 147, 123, 128, 75 },  //14
                                         { 225, 200, 28, 27 },   //15
                                         { 146, 193, 146, 29 },  //16
                                         { 192, 51, 148, 29 },   //17
                                         { 131, 156, 84, 75 },   //18
                                         { 241, 55, 47, 27 },    //19
                                         { 192, 229, 232, 29 },  //20
                                         { 32, 107, 165, 81 },   //21
                                         { 162, 89, 115, 29 },   //22
                                         { 16, 166, 117, 81 },   //23
                                         { 192, 87, 59, 29 } };  //24


void rfid_settings() {
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  //ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
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
  //initialize wifi
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();
  Serial.begin(115200);
  Serial.println("Initialize RFID");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);  // CHANGE DEFAULT PINS
  rfid.PCD_Init();
  Serial.print("Reader :");
  rfid.PCD_DumpVersionToSerial();
  Serial.println("RFID ready");
  Serial.println(rfid.PCD_GetAntennaGain());  // gives me a 64 (00000010)
  rfid.PCD_SetAntennaGain(112);               // set to max (00001110)
  Serial.println(rfid.PCD_GetAntennaGain());  //gives me the 112
  rfid_settings();
  Serial.println("Please pass gabarit card");
}

void loop() {
  int id = readRFID();

/*
  if ((id != -1) && (id != 0)) {
    sendInfo(id);
    current = millis();
    lastSend = id;
  }

  if (millis() - current > 5000) {
    if (lastSend != 0) {
      Serial.println("too long since seen gabarit");
      sendInfo(0);
      lastSend = 0;
      current = millis();
    }
  }*/
  delay(1000);
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
  Serial.print("Sending id: ");
  Serial.println(task);
  const int data_items = 1;

  Telemetry data[data_items] = {
    { "task", task },
  };
  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}




int getCardId(byte* buffer, int size) {
  String buffer_str = "";
  int card_id = -1;
  int isequal = 0;  //aux variable to compare rfid numbers
  int num_of_cards = 0;
  num_of_cards = num_of_gabarits;

  int curr_rfid[4] = { 0, 0, 0, 0 };
  //buffer[0] = 50 buffer[1] = 221 bufer[2] = 196 buffer[3] = 77
  for (int i = 0; i < size; i++) {
    buffer_str = String(buffer[i]);  //buffer[i] is byte type, need to pass first to a string
    curr_rfid[i] = buffer_str.toInt();
  }

  for (int i = 0; i < num_of_cards; i++) {
    isequal = compareRfid(curr_rfid, rfid_gabarit[i], size);
    if (isequal) {
      card_id = i + 1;  //since break is 0, we add 1 to each task so they are numbered from 1 to 6
      break;
    }
  }

  return card_id;
}


bool compareRfid(int rfid1[], int rfid2[], int size) {
  int equal = 0;
  for (int i = 0; i < size; i++) {
    if (rfid1[i] == rfid2[i])
      equal++;
  }
  if (equal == size)
    return true;
  else
    return false;
}

//Helper routine to dump a byte array as dec values to Serial.
void printDec(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.println("");
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
  Serial.println();
}

int readRFID() {
  //reading
  ////Read RFID card
  int id = -1;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent()) {
    //Serial.println("no, no new card is present");
    return -1;
  }
  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial()) {
    Serial.println("NUID has been read");
    return -1;
  }
  // Store NUID into nuidPICC array

  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  //Serial.print("RFID In dec: ");
  //printDec(rfid.uid.uidByte, rfid.uid.size);


  id = getCardId(rfid.uid.uidByte, rfid.uid.size);
  if (id == -1) {
    Serial.println("Gabarit not found");
  } else {
    Serial.print("Gabarit passing: ");
    Serial.println(id);
  }
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return id;
}
