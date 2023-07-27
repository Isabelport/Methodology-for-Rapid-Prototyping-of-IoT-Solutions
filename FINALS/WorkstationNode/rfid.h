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
int first_rfid = 1;

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
const int size_of_rfid = 11;
/*
int rfid_employee[num_of_employees][4] = { { 242, 203, 81, 76 },   //11
                                           { 163, 163, 30, 77 },   //12
                                           { 147, 231, 204, 77 },  //13
                                           { 147, 123, 128, 75 },  //14
                                           { 225, 200, 28, 27 },   //15
                                           { 146, 193, 146, 29 },  //16
                                           { 192, 51, 148, 29 },   //17
                                           { 131, 156, 84, 75 },   //18
                                           { 241, 55, 47, 27 },    //19
                                           { 192, 229, 232, 29 },  //20
                                           { 32, 107, 165, 81 },   //21 NAOOOO
                                           { 162, 89, 115, 29 },   //22
                                           { 16, 166, 117, 81 },   //23
                                           { 192, 87, 59, 29 } };  //24*/
String rfid_employee_junto[num_of_employees] = { "2422038176",   //11
                                                 "1631633077",   //12
                                                 "14723120477",  //13
                                                 "14712312875",  //14
                                                 "2252002827",   //15
                                                 "14619314629",  //16
                                                 "1925114829",   //17
                                                 "1311568475",   //18
                                                 "241554727",    //19
                                                 "19222923229",  //20
                                                 "3210716581",   //21
                                                 "1628911529",   //22
                                                 "1616611781",   //23
                                                 "192875929" };  //24

String rfid_employee_real[num_of_employees] = { "0007263753",    //1
                                                "0007263832",    //2
                                                "0002157941",    //3
                                                "0007263831",    //4
                                                "0007263893",    //5
                                                "0007263988",    //6
                                                "0007263755",    //7
                                                "0007263835",    //8
                                                "0007263752",    //9
                                                "0007263833",    //10
                                                "0007263795",    //11
                                                "0007263795",    //12
                                                "0007232194",    //13
                                                "0002152644" };  //14

String employee_name[num_of_employees + 1] = { "", "Antonio", "Daniela", "Diogo", "Elisabete", "Isabel", "Joao", "Joaquim", "Lidia",
                                               "Maria", "Mario", "Patricia", "Raquel", "Ana", "Paulo" };  //last one is for unrecognized employee

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

task data[8];  //6 structs of task

void initRFIDSensor() {
  rfid.PCD_Init();
  Serial.print("Reader :");
  rfid.PCD_DumpVersionToSerial();
  bool test = rfid.PCD_PerformSelfTest();
  if (test) {
    Serial.println("RFID ready!");
    tft.drawString("RFID conectado", 5, 40, 4);
    delay(1500);
  } else {
    while (!rfid.PCD_PerformSelfTest()) {
      Serial.println("RFID failed to begin. Please check wiring. Freezing...");
      tft.drawString("Verificar RFID ", 5, 40, 4);
      delay(500);
    }
    Serial.println("RFID ready!");
    tft.fillRect(10, 50, 40, 100, TFT_BLACK);
    tft.drawString("RFID conectado", 5, 40, 4);
    delay(1500);
  }
}



void printDec(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
  Serial.println();
}

bool compareRfid_employee(String rfid1, String rfid2) {
  int equal = 0;
  Serial.println("compare rfid employee");
  Serial.println(rfid1);
  Serial.println(rfid2);
  if (rfid1 == rfid2)
    return true;
  else
    return false;
}


bool compareRfid(int rfid1[], int rfid2[], int size) {
  int equal = 0;
  Serial.println("Compare rfid task");

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

  if ((mode == "task_stat1") || (mode == "task_stat2")) {
    num_of_cards = num_of_tasks;
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
      if (isequal) {
        card_id = i;  //since break is 0, we add 1 to each task so they are numbered from 1 to 6
        break;
      }
    }
  }

  else if (mode == "employee") {
    num_of_cards = num_of_employees;
    String curr_rfid = "";
    for (int i = 0; i < size; i++) {
      buffer_str = String(buffer[i]);  //buffer[i] is byte type, need to pass first to a string
      curr_rfid = curr_rfid + buffer_str.toInt();
    }
    for (int i = 0; i < num_of_cards; i++) {
      isequal = compareRfid_employee(curr_rfid, rfid_employee_junto[i]);
      if (isequal) {
        card_id = i;  //since break is 0, we add 1 to each task so they are numbered from 1 to 6
        break;
      }
    }  
  }

  card_id = card_id + 1;

  if (card_id == -1) {
    if (mode == "employee")  //even if it does not recognize in database, it is still allowed to move on
      card_id = 0;
    else
      card_id = 7;
  }



  return card_id;
}


// RFID functions
int readRFID(String mode) {  //mode TASK or EMPLOYEE

  //making sure that if rfid looses conection it gives a warning!
  bool test = rfid.PCD_PerformSelfTest();
  tft.setTextColor(TFT_WHITE);
  if (mode == "employee") {  //first_rfid ==1

    tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
    if (!test) {  //CHANGE TO !TEST
      //if(rfid.PCD_PerformSelfTest()){
      while (!rfid.PCD_PerformSelfTest()) {  //MUDAR PARA !
        tft.setTextColor(TFT_BLUE);
        Serial.println("RFID failed to begin. Please check wiring. Freezing...");
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Verificar RFID ", 5, 60, 4);
        delay(500);
      }
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_WHITE);
      Serial.println("RFID ready 3!");
      tft.drawString("RFID conectado", 5, 60, 4);
      delay(1500);
    }
  }
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

  if (mode == "task") {
    id = getCardId(rfid.uid.uidByte, rfid.uid.size, "task_stat1");
    id = getCardId(rfid.uid.uidByte, rfid.uid.size, "task_stat2");
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
  first_rfid = 0;
  return id;
}

task assignTaskValues(task data[]) {  // id 0-6

  //0 is break
  data[0].task_name = "Pausa";  //break
  data[0].av = 0;
  data[0].pr = 0;

  data[1].task_name = "TEKOX Cabo P&V";
  data[1].av = 12;
  data[1].pr = 0;

  data[2].task_name = "TEKOX Cabo A&C";
  data[2].av = 12;
  data[2].pr = 0;

  data[3].task_name = "Montagem MFALG";
  data[3].av = 144;
  data[3].pr = 0;

  data[4].task_name = "Parafusos lentes";
  data[4].av = 40;
  data[4].pr = 0;

  data[5].task_name = "";  //activity __
  data[5].av = 0;
  data[5].pr = 0;

  data[6].task_name = "";  //activity __
  data[6].av = 0;
  data[6].pr = 0;

  data[7].task_name = "";  //unknown activity
  data[7].av = 0;
  data[7].pr = 0;

  return data[8];
}
