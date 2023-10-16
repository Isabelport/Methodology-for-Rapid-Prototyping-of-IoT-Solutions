//Libraries
#include <SPI.h>          //https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>      //https://github.com/miguelbalboa/rfid
#include <WiFi.h>         // WiFi control for ESP32
#include <ThingsBoard.h>  // ThingsBoard SDK
#include "secrets.h"
//Constants

//#define implementation

#define SS_PIN 5
#define RST_PIN 3
#define MISO_PIN 19
#define MOSI_PIN 23
#define SCK_PIN 18
//Variables
byte nuidPICC[4] = { 0, 0, 0, 0 };
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
int id = -1;
int last_send = 0;
unsigned long sampling_time = 2000;
unsigned long max_time = 5000;
unsigned long last_seen_in = 0;
unsigned long current = 0;

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
int rfid_gabarit[num_of_gabarits][4] = { { 225, 175, 204, 27 },  //1 
                                         { 161, 197, 72, 72 },   //2 
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
  Serial.println(rfid.PCD_GetAntennaGain());  // gives me a 64 (00000010)
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);   // set to max (00001110)
  Serial.println(rfid.PCD_GetAntennaGain());  //gives me the 112
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
#ifndef implementation
  Serial.begin(115200);
#endif
  Serial.println("Initialize RFID");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);  // CHANGE DEFAULT PINS
  rfid.PCD_Init();
  Serial.print("Reader :");
  rfid.PCD_DumpVersionToSerial();
  Serial.println("RFID ready");
  rfid_settings();
  Serial.println("Please pass gabarit card");
  
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();
  connectTB();
}

void loop() {

  int id = readRFID();
  //delay(100);

  if (id != -1) {
    if (last_send != id) {
      sendInfo(id);
      Serial.print("1: ");
      Serial.println(id);
      last_seen_in = millis();
      last_send = id;
    } else if (millis() - last_seen_in >= sampling_time) {  //send repeated message "2","2", with a rate of sampling_time
      sendInfo(id);
      Serial.print("2: ");
      Serial.println(id);
      last_seen_in = millis();
    }
  }

  if ((millis() - last_seen_in >= max_time) && (last_send != 0)) {
    sendInfo(0);
    Serial.print("3: ");
    Serial.println(0);
    last_send = 0;
  }

  rfid.PCD_SoftPowerDown();
  rfid.PCD_SoftPowerUp();
}


int readRFID() {
  ////Read RFID card
  int id = -1;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent())
    return -1;
  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return -1;
  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  printDec(rfid.uid.uidByte, rfid.uid.size);
  id = getCardId(rfid.uid.uidByte, rfid.uid.size);
  

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return id;
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

void connectTB() {
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

void sendInfo(int gabarit) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();

  Serial.print("Sending id: ");
  Serial.println(gabarit);
  const int data_items = 1;

  Telemetry data[data_items] = {
    { "gabarit", gabarit },
  };
  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}

//Helper routine to dump a byte array as dec values to Serial.
void printDec(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
    Serial.print(", ");
  }
  Serial.println();
}
