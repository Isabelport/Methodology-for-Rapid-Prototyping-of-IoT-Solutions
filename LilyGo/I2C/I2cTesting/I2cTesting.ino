#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid

#define INTERRUPT_PIN 3
#define SHUTDOWN_PIN 2
#define SDA_PIN 10
#define SCL_PIN 12
#define SS_PIN 10
#define SCK_PIN 12
#define RST_PIN 18
#define MISO_PIN 13
#define MOSI_PIN 11


//SFEVL53L1X distanceSensor;

//Uncomment the following line to use the optional shutdown and interrupt pins.
SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

//Parameters RFID
/*
const int ipaddress[4] = {103, 97, 67, 25};
//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);*/

void setup()
{
  Serial.begin(115200);
  Serial.println("start");
  delay(4000);
  //shutdown e output, interrupt e input
  
  Wire.begin(SDA_PIN, SCL_PIN); //SDA SCL

  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);  
  //pinMode(INTERRUPT_PIN, OUTPUT);
  //digitalWrite(INTERRUPT_PIN, HIGH);
  //pinMode(INTERRUPT_PIN, INPUT);

  
  Serial.println("VL53L1X Qwiic Test");

  if (distanceSensor.begin() != 0) { //Begin returns 0 on a good init
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Sensor online!");
  distanceSensor.setDistanceModeLong();

  //RFID
  /*
  Serial.println("Initialize System");
 	SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN); // CHANGE DEFAULT PINS
 	rfid.PCD_Init();
 	Serial.print("Reader :");
 	rfid.PCD_DumpVersionToSerial();
*/
  i2c_address();
}

void loop()
{
  readToF();
  Serial.println();
  //readRFID();
}

void readToF(){
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady())
  {
    delay(1);
  }
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();

  Serial.print("Distance(mm): ");
  Serial.print(distance);

}

/*
void readRFID(void ) { 
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
 	Serial.println();
 	// Halt PICC
 	rfid.PICC_HaltA();
 	// Stop encryption on PCD
 	rfid.PCD_StopCrypto1();
}
*/


//Helper routine to dump a byte array as dec values to Serial.

void printDec(byte *buffer, byte bufferSize) {
 	for (byte i = 0; i < bufferSize; i++) {
 			Serial.print(buffer[i] < 0x10 ? " 0" : " ");
 			Serial.print(buffer[i], DEC);
 	}
}

void i2c_address() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}
