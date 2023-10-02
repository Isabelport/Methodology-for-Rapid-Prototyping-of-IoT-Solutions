#include <SPI.h>      //https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>  //https://github.com/miguelbalboa/rfid

//Pins
#define SS_PIN 10
#define RST_PIN 3
#define MISO_PIN 13
#define MOSI_PIN 11
#define SCK_PIN 12

int task_id = -1;
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

task data[8];  //6 structs of task


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


void initRFIDSensor() {
  rfid.PCD_Init();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  bool test = rfid.PCD_PerformSelfTest();
  if (test) {
    Serial.println(F("RFID ready!"));
    tft.drawString("RFID conectado", 5, 40, 4);
    delay(1500);
  } else {
    while (!rfid.PCD_PerformSelfTest()) {
      Serial.println(F("RFID failed to begin. Please check wiring. Freezing..."));
      tft.drawString("Verificar RFID ", 5, 40, 4);
      delay(500);
    }
    Serial.println(F("RFID ready!"));
    tft.fillRect(10, 50, 40, 100, TFT_BLACK);
    tft.drawString("RFID conectado", 5, 40, 4);
    delay(1500);
  }
}



void printDec(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i]);
  }
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

int getCardId(byte* buffer, int size, String mode) {  //mode = task_stat1, task_stat2 
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
        card_id = i;  
        break;
      }
    }
  }

  card_id = card_id + 1;  //since break is 0, we add 1 to each task so they are numbered from 1 to 6
  return card_id;
}


// RFID functions
int readRFID() {  //mode TASK 

  //making sure that if rfid looses conection it gives a warning!
  bool test = rfid.PCD_PerformSelfTest();
  tft.setTextColor(TFT_WHITE);
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
  Serial.println(F("RFID In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);


    id = getCardId(rfid.uid.uidByte, rfid.uid.size, "task_stat1");
    Serial.print(F("id1:"));
    Serial.println(id);
    if (id == 0) // if still did not find, search on task 2
      id = getCardId(rfid.uid.uidByte, rfid.uid.size, "task_stat2");
    Serial.print(F("id2:"));
    Serial.println(id);
    if (id == 0) {
      id = 7;
    }
    Serial.print(F("Task being performed: "));
    Serial.println(id);


  

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return id;
}

////////////////////////////////////////////////////MUDAR NOMES E TEMPOS DE TAREFAS"///////////////////////////////////////////////////////////////
task assignTaskValues(task data[]) {  // id 0-6

  //0 is break
  data[0].task_name = "Pausa";  //NAO MUDAR
  data[0].av = 0;
  data[0].pr = 0;

  data[1].task_name = "TEKOX Cabo P&V";//"Preparacao Tampa";
  data[1].av = 12;
  data[1].pr = 0;

  data[2].task_name = "TEKOX Cabo A&C"; //"Tekox Cabo A&C+Terra";
  data[2].av = 12;
  data[2].pr = 0;

  data[3].task_name = "Montagem MFALG"; //"Montagem Modulo";
  data[3].av = 144;
  data[3].pr = 0;

  data[4].task_name = "Parafusos lentes"; //"Lente e chips";
  data[4].av = 40;
  data[4].pr = 0;

  data[5].task_name = ""; //activity __ "Porca em PCB";";  
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
