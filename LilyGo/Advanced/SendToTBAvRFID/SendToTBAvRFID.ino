#include <WiFi.h> // WiFi control for ESP32
//#define THINGSBOARD_ENABLE_PROGMEM 0
#include <ThingsBoard.h>  // ThingsBoard SDK
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid


//Font header files
#include "DSEG7_Classic_Regular_50.h"
#include "DSEG7_Classic_Regular_12.h"
#include "DejaVu_LGC_Sans_Bold_12.h"
// Include the header files that contain the icons
#include "Down.h"
#include "Equal.h"
#include "Up.h"
#include "Star.h"
// Include the header files that contain wi-fi password
#include "secrets.h"

//RFID Constants
#define SS_PIN 10
#define RST_PIN 18
#define MISO_PIN 13
#define MOSI_PIN 11
#define SCK_PIN 12

#define yellow 0x65DB

//Time related
int laps = 0;
int sum = 0;
long ss1 = 0; //aux to count milliseconds
long ss2 = 0;  
int ss = 0;  //miliseconds
int s = 0;   //seconds
int m = 0;   //minutes
String seconds, minutes; //minutes and seconds of current task
String current, temp; //current time and auxiliar variable
int tt_h = 0;
int tt_m = 0; //total hours and minutes of certain task
int tt_s = 0;
int tt_ss = 0;
String tt_hours, tt_minutes, tt_seconds;
String tt_current, tt_temp;
int laptime = 0; //laptime in seconds
int laptime_m = 0;
int laptime_s = 0;

//Interface 
int first = 1;

//value given by rfid
int id = -1;

//task related
struct task {
  String task_name;
  float av; //in seconds (ex: 124)
  int pr;
};
int av; // average in integer

task data[5]; //4 structs of task

//CHANGE ACCORDING TO TASK
int treshold = 1; //time of difference between an increase or decrease in relation to average of time of certain task 

//buttons
int pom = 0;  
int pom2 = 0;

//RFID parameters
const int ipaddress[4] = {103, 97, 67, 25};
//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
int rfid_on = 0;

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;

/*******************************************CODE*********************************************/

void assign_values(){ // id 0-4

  data[0].task_name = "Cabos";
  data[0].av = 0; // in seconds
  data[0].pr = 0; // personal record

  data[1].task_name = "Fios";
  data[1].av = 0;
  data[1].pr = 0;

  data[2].task_name = "Parafusos";
  data[2].av = 4;
  data[2].pr = 0;

  data[3].task_name = "Luz";
  data[3].av = 5;
  data[3].pr = 0;

  data[7].task_name = "Teste";
  data[7].av = 6;
  data[7].pr = 0;

  return;
}

void setup() {
  //Initialize screen
  tft.init();
  tft.setRotation(1); //1 horizontal cabo do lado esquerdo // 3 horizontal cabo do lado direito
  tft.fillScreen(TFT_BLACK);
  Serial.begin(115200);
  Serial.println("TFT ready");
  tft.setSwapBytes(true);
  //Initialize buttons
  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);
  delay(1000);

  tft.drawString("Por favor ligar-se ao Wi-Fi.", 14, 70, 4);

  //Initialize wifi and TB
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();
  connectTB();

  //Initialize RFID
  if (rfid_on){
    Serial.println("Initialize System");
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN); // CHANGE DEFAULT PINS
    rfid.PCD_Init();
    Serial.print("Reader :");
    rfid.PCD_DumpVersionToSerial();
  }

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
  
  assign_values();
  waitfortask_screen();
  //Serial.println("go to loop");
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

void connectTB(){
  // Reconnect to ThingsBoard, if needed
  while (!tb.connected()) {
    Serial.println("Connecting to The ThingsBoard");
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      delay(500);  //return;
    }
  }
}

void sendInfo(int min, int sec, int task) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();
  //Serial.println("-> Sending data...");
  const int data_items = 3;
  Telemetry data[data_items] = {
    { "Minutes", min },
    { "Seconds", sec },
    { "task", task },
    };

  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}

void sendInfo_final(int av, int pr, int total_h, int total_min) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  connectTB();

  const int data_items = 4;
  Telemetry data[data_items] = {
    { "Average", av },
    { "Personal Record", pr },
    { "total_h", total_h },
    { "total_min", total_min },
    };

  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}

void reset_tasktime(){ 
  tft.fillRect(10, 70, 120, 60, TFT_BLACK);
  //tft.drawString(current, 10, 70); //task time
  s = 0;
  m = 0;
}

void reset() {
  first = 0;
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);

  tft.drawString("TAREFA:", 10, 7);
  tft.drawString(data[id].task_name, 75, 7);

  tft.drawString("MEDIA:", 210, 7);
  av = round(data[id].av);
  int av_m = get_min(av);
  int av_s = get_sec(av);
  String av_m_ = time_to_string(av_m);
  String av_s_ = time_to_string(av_s);
  if (av != 0)
    tft.drawString(av_m_ + ":" + av_s_, 265, 7);

  tft.drawLine(200,30,200,200,yellow);
  tft.drawLine(0,30,500,30,yellow);

  laps = 0;
  data[id].pr = 0;
  tt_s = 0;
  tt_m = 0;
  tt_h = 0;
  sum = 0;
}

void waitfortask_screen(){
  tft.fillScreen(TFT_BLACK);
  Serial.println("Wait for task Screen");
  if (first == 0){
    tft.drawString("Pausa", 14, 45, 4);
    tft.drawString("Para comecar", 14, 70, 4);
    tft.drawString("passar cartao da tarefa.", 14, 95, 4);
  }
  else{
    tft.drawString("Para comecar", 14, 60, 4);
    tft.drawString("passar cartao da tarefa.", 14, 85, 4);
  }
  while(1){
    if (rfid_on){
      readRFID();
    }
    else
    {
      delay(2000);
      id = 0;
    }
    if (id != -1){
      Serial.println("Card found");
      break_screen();
      break;
    }
    if (digitalRead(0) == 0){
      if (pom == 0){
        pom = 1;
      }
      }
      else {
      pom = 0;
      }
    if (digitalRead(14) == 0) {
      if (pom2 == 0){;
        pom2 = 1;
      }
    }
    else{
      pom2 = 0;
    }
  } 
}


void break_screen(){
  tft.fillScreen(TFT_BLACK);
  Serial.println("Break screen");

  tft.drawString("Tarefa atual: ", 14, 20, 4);
  tft.drawString(data[id].task_name, 160, 20, 4);

  tft.drawString("Clicar no botao de cima", 14, 50, 4);
  tft.drawString("para trocar de tarefa", 14, 75, 4);

  tft.drawString("ou no botao de baixo", 14, 105, 4);
  tft.drawString("para comecar", 14, 130, 4);

  while(1){
    // bottom button
    if (digitalRead(0) == 0){
      if (pom == 0){
        pom = 1;
        reset();
        break;
      }
      }
      else {
      pom = 0;
      }
    // up button
    if (digitalRead(14) == 0) {
      if (pom2 == 0){
        id = -1;
        pom2 = 1;
        waitfortask_screen();
        break;
      }
    }
    else{
      pom2 = 0;
    }
    }
}


void loop() {
    ss1 = millis();
    
    minutes = time_to_string(m);
    seconds = time_to_string(s);

    tt_hours = time_to_string(tt_h);
    tt_minutes = time_to_string(tt_m);
    tt_seconds = time_to_string(tt_s);
    //tt_seconds = time_to_string(tt_s);


    current = minutes + ":" + seconds;
    //tt_current = tt_hours + ":" + tt_minutes + ":" + tt_seconds;
    tt_current = tt_hours + ":" + tt_minutes;
    //Serial.println(current);
    if (current != temp) {
      tft.setFreeFont(&DSEG7_Classic_Regular_50);
      tft.drawString(current, 10, 70);
      temp = current;
    }

    if (tt_current != tt_temp) {
      
      //tft.drawString("Total time:",10, 130);
      tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
      //tft.setFreeFont(&DSEG7_Classic_Regular_12);
      tft.drawString("TOTAL:", 10, 130);
      //tft.setFreeFont(&DSEG7_Classic_Regular_12);
      tft.drawString(tt_current, 75, 130);
      tt_temp = tt_current;
    }
  
  // MILLISECONDS
  if (ss1 > ss2 + 9) { //counting milliseconds
    ss2 = ss1;
    ss++;
    tt_ss++;
  }

  // SECONDS
  if (ss > 99) { //counting seconds for task time
    s++;
    ss = 0;
  }

  if (tt_ss > 99) { //counting seconds for total time
    Serial.print("Task time: "); Serial.print(current); Serial.print("  Total time:"); Serial.print(tt_current);Serial.print(":");Serial.println(tt_seconds);
    tt_s++;
    tt_ss = 0;
  }

  // MINUTES
  if (s > 59) {
    s = 0;
    m++;
  }

  if (tt_s > 59) {
    tt_s = 0;
    tt_m++;
  }

  // HOURS
  if (tt_m > 59) {
    tt_m = 0;
    tt_h++;
  }

  
  buttons();
}

String time_to_string(int t){
  String ts;
  if (t < 10) ts = "0" + String(t);
  else ts = String(t);
  return ts;
}

int get_min(int total){
  int min = total / 60;
  return min;  
}

int get_sec(int total){
  int sec = total % 60;
  return sec;
}

void average_func(){
  sum = sum + laptime;
  float av_float;
  //if (data[id].av == round( sum / laps)) return; //average stays the same
  Serial.print("id: ");Serial.print(id);Serial.print("   av: ");Serial.print(round(data[id].av));Serial.print("   pr: ");Serial.println(data[id].pr);

  //Serial.println(id);
  //Serial.print("Average: "); Serial.println(data[id].av);
  Serial.print("Sum: "); Serial.print(sum); Serial.print("   Laps: "); Serial.println(laps);

  if (data[id].av == 0){ //when average is not defined yet (== 0)
    Serial.print("Aaverage updated: "); Serial.print(data[id].av); Serial.print(" --> "); 
    data[id].av = sum / laps;
  }
  else{
    /**/
    //av_float = data[id].av * 0.8 + sum / laps * 0.2;
    //Serial.println(av_float);
    /**/
    //if (data[id].av == round( data[id].av * 0.8 + sum / laps * 0.2 )) return; 
    Serial.print("Average updated: "); Serial.print(data[id].av); Serial.print(" --> "); 
    data[id].av = data[id].av * 0.8 + sum / laps * 0.2 ; //data[id].av = sum / laps; // 124s
    

  }
  Serial.print(data[id].av);
  av = round(data[id].av);
  Serial.print(" --> "); Serial.println(av);
  int av_m = get_min(data[id].av);
  int av_s = get_sec(data[id].av);
  String av_m_ = time_to_string(av_m);
  String av_s_ = time_to_string(av_s);

  //Serial.println(av_m_ + ":" + av_s_);
  tft.fillRect(265,7,80 ,20 ,TFT_BLACK);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
  tft.drawString(av_m_ + ":" + av_s_, 265, 7);

}

bool personal_record(){
  if (data[id].pr == 0){ //personal record not defined yet
    data[id].pr = laptime; 
    return 0;
  }
  else if (laptime < data[id].pr){ //better time than personal record
    data[id].pr = laptime;
    return 1;
  }
  else 
    return 0;
}

// displays image and difference between current lap and average of that task
void score_time(){
  // difference between laptime and average time it takes to perform task

  int diff = laptime - round(data[id].av); // 138 - 124 = 14 s (worse) 110 - 124 = -14 s (better)
  int minus = 0;
  if (diff < 0)  minus = 1;
  
  //Serial.print("min: "); Serial.print(min); Serial.print(" sec: "); Serial.println(sec);
  Serial.print("Laptime: "); Serial.println(laptime);
  Serial.print("Diff: "); Serial.println(diff); 
  int diff_m = get_min(diff);
  int diff_s = get_sec(diff);

  if (diff_s < 0 )
    diff_s = - diff_s;
  if (diff_m < 0 )
    diff_m = - diff_m; 

  String diff_m_ = time_to_string(diff_m);
  String diff_s_ = time_to_string(diff_s);

  int x_image = 245;
  int y_image = 65;
  int x_text = 220;
  int y_text = 105;

  if (personal_record()){
    tft.pushImage (x_image, y_image, 32, 32, star); 
    tft.fillRect( x_text, y_text, 35, 100, TFT_BLACK);
    if (minus) tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
    else tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //DECREASE IN TIME -> DOWN
  else if (diff < - treshold){ //if difference is less than -treshold than there is big decrease of the laptime in relation to average (better)
    //Serial.println("down");  
    tft.pushImage (x_image, y_image, 32, 32, down);
    tft.fillRect( x_text, y_text, 35, 100, TFT_BLACK);
    tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //INCREASE IN TIME -> UP
  else if(diff > treshold){ //if difference is bigger than treshold than there is big increase of the laptime in relation to average (worse)
    //Serial.println("up");
    tft.pushImage (x_image, y_image, 32, 32, up); 
    tft.fillRect( x_text, y_text, 35, 100, TFT_BLACK);
    tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //EQUAL
  else{ // if difference is between -treshold and treshold, difference is not considerably big 
    //Serial.println("equal");
    tft.pushImage (x_image, y_image, 32, 32, equal); //32 32
    tft.fillRect( x_text, y_text, 35, 100, TFT_BLACK);
    if (minus) tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
    else tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
    
  }
}

void buttons() {
  if (digitalRead(0) == 0) {
    if (pom == 0) {
      laps++;

      laptime_m = m;
      laptime_s = s;
      laptime = laptime_m * 60 + laptime_s;

    
      Serial.print("Laptime: "); Serial.print(laptime_m); Serial.print(":"); Serial.println(laptime_s);

      //lap time is always positive
      if (laptime_m < 0)
        laptime_m = laptime_m * -1;
        
      if (laptime_s < 0)
        laptime_s = laptime_s * -1;

      score_time(); 
      average_func();

      sendInfo(laptime_m, laptime_s, id);
      reset_tasktime();
      pom = 1;
    }
  }
    else {
      pom = 0;
    }

  if (digitalRead(14) == 0) {
    if (pom2 == 0){
      id = -1;
      sendInfo_final(round(data[id].av), data[id].pr, m, s);
      waitfortask_screen();
      pom2 = 1;
    }
  }
  else{
    pom2 = 0;
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


