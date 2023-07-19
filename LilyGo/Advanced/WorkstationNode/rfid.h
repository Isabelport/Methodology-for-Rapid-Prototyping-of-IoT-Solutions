#include <SPI.h>      //https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>  //https://github.com/miguelbalboa/rfid

//Pins
#define SS_PIN 10
#define RST_PIN 3
#define MISO_PIN 13
#define MOSI_PIN 11
#define SCK_PIN 12

//task id
int id = -1;
//

//parameters
const int ipaddress[4] = { 103, 97, 67, 25 };
//Variables
byte nuidPICC[4] = { 0, 0, 0, 0 };
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
int rfid_on = 1;  //turn off(0)/on(1) rfid sensor for testing purposes

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

//task related
struct task {
  String task_name;
  float av;  //in seconds (ex: 124)
  int pr;
};

task data[5];  //5 structs of task


void rfid_settings() {
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
}

//returns hashtag of the rfid uuid
int hashFunc(byte *buffer, int size) {
  String buffer_str = "";
  int buffer_i = 0;
  int buffer_sum = 0;
  int tag = -1;

  //50 221 196 77
  for (int i = 0; i < size; i++) {
    buffer_str = String(buffer[i]);
    buffer_i = buffer_str.toInt();
    while (buffer_i > 0) {
      buffer_sum = buffer_sum + buffer_i % 10;
      buffer_i = buffer_i / 10;
    }
  }

  tag = buffer_sum % 5;  //40 % 5 = 0
  Serial.print("Hashtag: ");
  Serial.println(tag);

  return tag;
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
  Serial.println();
}

// RFID functions
void readRFID(void) { /* function readRFID */
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
  Serial.print("RFID In dec: ");
  printDec(rfid.uid.uidByte, rfid.uid.size);
  id = hashFunc(rfid.uid.uidByte, rfid.uid.size);
  Serial.print("Name:"); Serial.print(data[id].task_name);
  Serial.print("   Av:"); Serial.print(round(data[id].av));
  Serial.print("   Pr:"); Serial.println(data[id].pr);
  Serial.print("Task being performed: ");
  Serial.println(id);

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

task assign_values(task data[5]) {  // id 0-4
  /*
  data[0].task_name = "Cabos";
  data[0].av = 10;  // in seconds
  data[0].pr = 0;   // personal record*/
  //0 is break

  data[1].task_name = "Fios";
  data[1].av = 25;
  data[1].pr = 0;

  data[2].task_name = "Parafusos";
  data[2].av = 4;
  data[2].pr = 0;

  data[3].task_name = "Tampa Can.";
  data[3].av = 7;
  data[3].pr = 0;

  data[4].task_name = "Soldar";
  data[4].av = 72;
  data[4].pr = 60;

  return data[5];
}
