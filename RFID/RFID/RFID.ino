//Libraries
#include <SPI.h>      //https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>  //https://github.com/miguelbalboa/rfid
//Constants
#define SS_PIN 10
#define RST_PIN 3
#define MISO_PIN 13
#define MOSI_PIN 11
#define SCK_PIN 12
//Parameters
const int ipaddress[4] = { 103, 97, 67, 25 };
//Variables
byte nuidPICC[4] = { 0, 0, 0, 0 };
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
int id = -1;

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

void rfid_settings() {
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  //ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
}


void setup() {
  //Init Serial USB
  Serial.begin(115200);
  Serial.println("Initialize RFID");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);  // CHANGE DEFAULT PINS
  rfid.PCD_Init();
  Serial.print("Reader :");
  rfid.PCD_DumpVersionToSerial();
  Serial.println("RFID ready");
  rfid_settings();
  Serial.println("Please pass employee card");

}

void loop() {
  readRFID("task_stat1");
  //delay(10000);
  //Serial.println("Please pass task card");
  //readRFID("task");
  //delay(10000);

}

//6 tasks of an rfid number of size 4, number of tasks can be adapted
//attention! task 0 is break
const int num_of_tasks = 6;
int rfid_task_stat1[num_of_tasks][4] = { { 41, 159, 54, 52 },     //1
                                   { 114, 115, 192, 75 },   //2
                                   { 225, 88, 162, 27 },    //3
                                   { 227, 24, 55, 52 },     //4
                                   { 147, 140, 181, 75 },   //5
                                   { 50, 221, 196, 77 } };  //6

int rfid_task_stat2[num_of_tasks][4] = { { 19, 163, 84, 17 },      //1
                                         { 225, 143, 202, 27 },    //2
                                         { 82, 150, 241, 77 },     //3
                                         { 154, 168, 54, 52 },     //4
                                         { 192, 97, 169, 29 },     //5
                                         { 115, 222, 188, 77 } };  //6

const int num_of_employees = 14;
int rfid_employee[num_of_employees][4] = { { 19, 163, 84, 17 },      //1
                                           { 225, 143, 202, 27 },    //2
                                           { 82, 150, 241, 77 },     //3
                                           { 227, 24, 55, 52 },      //4
                                           { 147, 140, 181, 75 },    //5
                                           { 50, 221, 196, 77 },     //6
                                           { 41, 159, 54, 52 },      //7
                                           { 114, 115, 192, 75 },    //8
                                           { 225, 88, 162, 27 },     //9
                                           { 227, 24, 55, 52 },      //10
                                           { 147, 140, 181, 75 },    //11
                                           { 50, 221, 196, 77 },     //12
                                           { 41, 159, 54, 52 },      //13
                                           { 114, 115, 192, 75 } };  //14


//gabarit is a working tray for the production line
const int num_of_gabarits = 24;
int rfid_gabarit[num_of_gabarits][4] = { { 19, 163, 84, 17 },      //1
                                          { 225, 143, 202, 27 },    //2
                                          { 82, 150, 241, 77 },     //3
                                          { 227, 24, 55, 52 },      //4
                                          { 147, 140, 181, 75 },    //5
                                          { 50, 221, 196, 77 },     //6
                                          { 41, 159, 54, 52 },      //7
                                          { 114, 115, 192, 75 },    //8
                                          { 225, 88, 162, 27 },     //9
                                          { 227, 24, 55, 52 },      //10
                                          { 147, 140, 181, 75 },    //11
                                          { 50, 221, 196, 77 },     //12
                                          { 41, 159, 54, 52 },      //13
                                          { 114, 115, 192, 75 },    //14
                                          { 227, 24, 55, 52 },      //15
                                          { 147, 140, 181, 75 },    //16
                                          { 50, 221, 196, 77 },     //17
                                          { 41, 159, 54, 52 },      //18
                                          { 114, 115, 192, 75 },    //19
                                          { 225, 88, 162, 27 },     //20
                                          { 227, 24, 55, 52 },      //21 //FODIDO MUDAR!!!!!!
                                          { 147, 140, 181, 75 },    //22
                                          { 50, 221, 196, 77 },     //23
                                          { 41, 159, 54, 52 }};     //24


int getCardId(byte* buffer, int size, String mode) { //mode = task_stat1, task_stat2 or employee
  String buffer_str = "";
  int card_id = -1;
  int isequal = 0;  //aux variable to compare rfid numbers
  int num_of_cards = 0;
  if ((mode == "task_stat1") || (mode == "task_stat2"))
    num_of_cards = num_of_tasks;
  else // mode == "employee"
    num_of_cards = num_of_employees;

  Serial.println("curr_rfid");
  int curr_rfid[4] = { 0, 0, 0, 0 };
  //buffer[0] = 50 buffer[1] = 221 bufer[2] = 196 buffer[3] = 77
  for (int i = 0; i < size; i++) {
    buffer_str = String(buffer[i]);  //buffer[i] is byte type, need to pass first to a string
    curr_rfid[i] = buffer_str.toInt();
    Serial.println(curr_rfid[i]);
  }

  for (int i = 0; i < num_of_cards; i++) {
    if (mode == "task_stat1")
      isequal = compareRfid(curr_rfid, rfid_task_stat1[i], 4);
    else if (mode == "task_stat2")
      isequal = compareRfid(curr_rfid, rfid_task_stat2[i], 4);
    else if (mode == "employee")
      isequal = compareRfid(curr_rfid, rfid_employee[i], 4);
    if (isequal) {
      Serial.println("yes! equal");
      Serial.println(i + 1);
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
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
  Serial.println();
}

void readRFID(String mode) { //mode 
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

  if (mode == "task_stat1") {
    id = getCardId(rfid.uid.uidByte, rfid.uid.size, mode);
    if (id == -1) {
      Serial.println("Task not found");
    } else {
    Serial.print("Task being performed: ");
    Serial.println(id);
    }
  }
  else if (mode == "task_stat2") {
    id = getCardId(rfid.uid.uidByte, rfid.uid.size, mode);
    if (id == -1) {
      Serial.println("Task not found");
    } else {
    Serial.print("Task being performed: ");
    Serial.println(id);
    }
  } else if (mode == "employee") {
    id = getCardId(rfid.uid.uidByte, rfid.uid.size, mode);
    if (id == -1) {
      Serial.println("Employee not found");
    } else {
      Serial.print("Employee working: ");
      Serial.println(id);
    }
  }
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

