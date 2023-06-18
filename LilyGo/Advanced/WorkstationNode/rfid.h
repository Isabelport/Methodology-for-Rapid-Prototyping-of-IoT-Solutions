#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid

//Pins
#define SS_PIN 10
#define RST_PIN 18
#define MISO_PIN 13
#define MOSI_PIN 11
#define SCK_PIN 12

//task id
int id = -1;

//parameters
const int ipaddress[4] = {103, 97, 67, 25};
//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
int rfid_on = 1; //turn off(0)/on(1) rfid sensor for testing purposes

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

void rfid_settings(){
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
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

void printDec(byte *buffer, byte bufferSize) {
 	for (byte i = 0; i < bufferSize; i++) {
 			Serial.print(buffer[i] < 0x10 ? " 0" : " ");
 			Serial.print(buffer[i], DEC);
 	}
}

// RFID functions
void readRFID(void ) { /* function readRFID */
 	////Read RFID card
 	for (byte i = 0; i < 6; i++) {
 			key.keyByte[i] = 0xFF;
 	}
 	// Look for new 1 cards
 	if ( ! rfid.PICC_IsNewCardPresent())
 			return;
 	// Verify if the NUID has been readed
 	if ( 	!rfid.PICC_ReadCardSerial())
 			return;
 	// Store NUID into nuidPICC array
 	for (byte i = 0; i < 4; i++) {
 			nuidPICC[i] = rfid.uid.uidByte[i];
 	}
 	Serial.print("RFID In dec: ");
 	printDec(rfid.uid.uidByte, rfid.uid.size);
  id = hashFunc(rfid.uid.uidByte,  rfid.uid.size);
  Serial.println();
  Serial.print("Task being performed: ");
  Serial.println(id);

 	// Halt PICC
 	rfid.PICC_HaltA();
 	// Stop encryption on PCD
 	rfid.PCD_StopCrypto1();
}
