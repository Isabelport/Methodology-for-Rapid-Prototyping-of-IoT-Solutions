#include <SPI.h>      //https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>  //https://github.com/miguelbalboa/rfid

//Pins
#define SS_PIN 10
#define RST_PIN 3
#define MISO_PIN 13
#define MOSI_PIN 11
#define SCK_PIN 12

int task_id = -1;
int emp_id = -1;
//

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
int rfid_employee[num_of_employees][4] = { { 242, 203, 81, 76 },   //11
                                           { 163, 163, 30, 77 },   //12
                                           { 147, 231, 204, 77 },  //13
                                           { 147, 123, 128, 75 },  //14
                                           { 225, 200, 28, 27 },   //15
                                           { 146, 193, 146, 29 },  //16
                                           { 192, 51, 148, 29 },  //17
                                           { 131, 156, 84, 75 },   //18
                                           { 241, 55, 47, 27 },    //19
                                           { 192, 229, 232, 29 },  //20
                                           { 227, 24, 55, 52 },    //21 NAOOOO
                                           { 162, 89, 115, 29 },   //22
                                           { 16, 166, 117, 81 },   //23
                                           { 192, 87, 59, 29 } };  //24

String employee_name[num_of_employees+1] = { "Ana", "Bernardo", "Carlota", "Duarte", "Emilia", "Francisco", "Gustavo",
                                           "Hugo", "Isabel", "Joao", "Leonor", "Maria", "Nuno", "Osvaldo", "" }; //last one is for unrecognized employee



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

task data[7];  //6 structs of task

void initRFIDSensor(){
  rfid.PCD_Init();
  Serial.print("Reader :");
  rfid.PCD_DumpVersionToSerial();
  bool test = rfid.PCD_PerformSelfTest();
  if (test){
    Serial.println("RFID ready!");
    tft.drawString("RFID conectado", 10, 50, 4);
    delay(1500);
  }
  else {
    while(!rfid.PCD_PerformSelfTest()){
      Serial.println("RFID failed to begin. Please check wiring. Freezing...");
      tft.drawString("Verificar RFID ", 10, 50, 4);
      delay(500);
    }
  }
}



void printDec(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
  Serial.println();
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

int getCardId(byte* buffer, int size, String mode) {  //mode = task_stat1, task_stat2 or employee
  String buffer_str = "";
  int card_id = -1;
  int isequal = 0;  //aux variable to compare rfid numbers
  int num_of_cards = 0;
  if ((mode == "task_stat1") || (mode == "task_stat2"))
    num_of_cards = num_of_tasks;
  else  // mode == "employee"
    num_of_cards = num_of_employees;

  int curr_rfid[4] = { 0, 0, 0, 0 };
  //buffer[0] = 50 buffer[1] = 221 bufer[2] = 196 buffer[3] = 77
  for (int i = 0; i < size; i++) {
    buffer_str = String(buffer[i]);  //buffer[i] is byte type, need to pass first to a string
    curr_rfid[i] = buffer_str.toInt();
  }

  for (int i = 0; i < num_of_cards; i++) {
    if (mode == "task_stat1")
      isequal = compareRfid(curr_rfid, rfid_task_stat1[i], 4);
    else if (mode == "task_stat2")
      isequal = compareRfid(curr_rfid, rfid_task_stat2[i], 4);
    else if (mode == "employee")
      isequal = compareRfid(curr_rfid, rfid_employee[i], 4);
    if (isequal) {
      card_id = i;  //since break is 0, we add 1 to each task so they are numbered from 1 to 6
      break;
    }
  }
  if((card_id == -1) && (mode == "employee" )) //even if it does not recognize in database, it is still allowed to move on
    card_id = 14;
  else if (card_id == -1)
    card_id = 6;

  return card_id;
}


// RFID functions
int readRFID(String mode) {  //mode
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
  } else if (mode == "task_stat2") {
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
  return id;
}

task assignTaskValues(task data[]) {  // id 0-6

  //0 is break


  data[0].task_name = "TEKOX Cabo P&V";
  data[0].av = 7.8;
  data[0].pr = 0;

  data[1].task_name = "TEKOX Cabo A&C";
  data[1].av = 7.8;
  data[1].pr = 0;

  data[2].task_name = "Montagem MFALG";
  data[2].av = 144;
  data[2].pr = 0;

  data[3].task_name = "Parafusos lentes";
  data[3].av = 45;
  data[3].pr = 0;

  data[4].task_name = "";
  data[4].av = 0;
  data[4].pr = 0;

  data[5].task_name = "";
  data[5].av = 0;
  data[5].pr = 0;

  data[6].task_name = "";
  data[6].av = 0;
  data[6].pr = 0;

  return data[7];
}
