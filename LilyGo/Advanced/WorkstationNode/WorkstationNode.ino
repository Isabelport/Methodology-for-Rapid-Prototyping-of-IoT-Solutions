#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include "fonts.h"
#include "icons.h"
// Include the header files that contain wi-fi password
#include "secrets.h"
#include "rfid.h"
#include "buttons.h"
#include "communication.h"
//#include "laptime.h"
//#include "tof.h"
//TOF
#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

//TOF constants
/*
#define INTERRUPT_PIN 43
#define SHUTDOWN_PIN 44
#define SDA_PIN 16
#define SCL_PIN 21*/
//SFEVL53L1X distanceSensor;
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

#define color_yellow 0x65DB

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
int break_m = 0; //counts the amount of time in break
int break_s = 2; //accounts for the delay it takes to start
int break_ss = 0;
//Interface 
int first = 1;

//task related
struct task {
  String task_name;
  float av; //in seconds (ex: 124)
  int pr;
};

task data[5]; //4 structs of task
//CHANGE ACCORDING TO TASK
int treshold = 1; //time of difference between an increase or decrease in relation to average of time of certain task 



/*******************************************CODE*********************************************/
void setup() {
  //Initialize screen
  tft.init();
  tft.setRotation(1); //1 horizontal cabo do lado esquerdo // 3 horizontal cabo do lado direito
  tft.fillScreen(TFT_BLACK);
  Serial.begin(115200);
  Serial.println("TFT ready");
  tft.setSwapBytes(true);
  //Initialize buttons
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);

  tft.drawString("Por favor ligar-se ao Wi-Fi.", 14, 70, 4);

  //Initialize wifi and TB
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();
  connectTB(); 

  //Initialize RFID
  if (rfid_on){
    Serial.println("Initialize RFID");
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN); // CHANGE DEFAULT PINS
    rfid.PCD_Init();
    Serial.print("Reader :");
    rfid.PCD_DumpVersionToSerial();
  }
  Serial.println("rfid check");
  rfid_settings();

  //Initialize ToF
  /*
  Wire.begin(SDA_PIN, SCL_PIN); //SDA SCL
  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);  
  Serial.println("Initialize ToF");
  */

  /*
  if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println("Sensor online!");
  distanceSensor.setDistanceModeLong();*/

  //Serial.println("ledc check");
  
  assign_values();
  Serial.println("lets");
  
  waitfortask_screen();
  
  //Serial.println("go to loop");
}

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

  data[4].task_name = "Teste";
  data[4].av = 6;
  data[4].pr = 0;

  return;
}

//zeros all variables
void reset() { 
  first = 0;
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);

  tft.drawString("TAREFA:", 10, 7);
  tft.drawString(data[id].task_name, 75, 7);

  tft.drawString("MEDIA:", 210, 7);
  int av = round(data[id].av);
  int av_m = get_min(av);
  int av_s = get_sec(av);
  String av_m_ = time_to_string(av_m);
  String av_s_ = time_to_string(av_s);
  if (av != 0)
    tft.drawString(av_m_ + ":" + av_s_, 265, 7);

  tft.drawLine(200,30,200,200,color_yellow);
  tft.drawLine(0,30,500,30,color_yellow);

  ss = 0;  
  s = 0;
  m = 0;
  laps = 0;
  data[id].pr = 0;
  tt_ss = 0;
  tt_s = 0;
  tt_m = 0;
  tt_h = 0;
  sum = 0;
  break_m = 0;
  break_s = 2;
  break_ss = 0;
}

//screen wait for task
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
    //since the function of readrfid takes a part of the processing, the seconds seem "slower",
    //so we assumed that 39 are 1 second, which in the end actually look like one second
    if (first==0){
      ss1 = millis();
      
      // MILLISECONDS
      if (ss1 > ss2 + 9) { //counting milliseconds for break time
        ss2 = ss1;
        break_ss++;
      }

      // SECONDS
      if (break_ss > 40) { //counting seconds for break time
        Serial.print("Break time: "); Serial.print(break_m);Serial.print(":");Serial.println(break_s);
        break_s++;
        break_ss = 0;
      }

      // MINUTES
      if (break_s > 59) { //counting minutes for break time
        break_s = 0;
        break_m++;
      }
    }
    
    if (rfid_on){
      readRFID();
    }
    else
    {
      //delay(500);
      id = 0;
    }
    if (id != -1){
      Serial.println("Card found");
      break_screen();
      break;
    }
    green = read_button(green, GREEN_BUTTON);
    yellow = read_button(yellow, YELLOW_BUTTON);
    //if (digitalRead(0) == 0){
    if (green.clicked){
      Serial.println("green");
      if (pom == 0){
        pom = 1;
      }
      }
      else {
      pom = 0;
      }
    //if (digitalRead(14) == 0) {
    if (yellow.clicked) {
      Serial.println("yellow");
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
  delay(1000);
  Serial.println("Break screen");
  tft.fillScreen(TFT_BLACK);

  tft.drawString("Tarefa atual: ", 14, 20, 4);
  tft.drawString(data[id].task_name, 160, 20, 4);

  tft.drawString("Clicar no botao amarelo", 14, 50, 4);
  tft.drawString("para trocar de tarefa", 14, 75, 4);

  tft.drawString("ou no botao verde", 14, 105, 4);
  tft.drawString("para comecar", 14, 130, 4);
  //Serial.println("mimim");

  while(1){
      if (first == 0){
      ss1 = millis();
      
      // MILLISECONDS
      if (ss1 > ss2 + 9) { //counting milliseconds for break time
        ss2 = ss1;
        break_ss++;
      }

      // SECONDS
      if (break_ss > 99) { //counting seconds for break time
        Serial.print("Break time: "); Serial.print(break_m);Serial.print(":");Serial.println(break_s);
        break_s++;
        break_ss = 0;
      }

      // MINUTES
      if (break_s > 59) { //counting minutes for break time
        break_s = 0;
        break_m++;
      }
    }
    green = read_button(green, GREEN_BUTTON);
    yellow = read_button(yellow, YELLOW_BUTTON);
    if (green.clicked){
      if (first == 0){
        sendInfo_TASK(break_m,break_s,0); //tell tb break is over
      }
      //restart task identification
      Serial.println("green");
      if (pom == 0){
        pom = 1;
        reset();
        break;
      }
      }
      else {
      pom = 0;
      }
    if (yellow.clicked){ 
      Serial.println("yellow");
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

    current = minutes + ":" + seconds;
    tt_current = tt_hours + ":" + tt_minutes + ":" + tt_seconds;
    if (current != temp) {
      tft.setFreeFont(&DSEG7_Classic_Regular_50);
      tft.drawString(current, 10, 70);
      temp = current;
    }

    if (tt_current != tt_temp) {
      tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
      tft.drawString("TOTAL:", 10, 130);
      //tft.drawString(tt_current, 75, 130);
      tft.drawString(tt_hours + ":" + tt_minutes, 75, 130);
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
    Serial.print("Task time: "); Serial.print(current); Serial.print("  Total time:"); Serial.println(tt_current);
    //Serial.print(":");Serial.println(tt_seconds);
    tt_s++;
    tt_ss = 0;
  }

  // MINUTES
  if (s > 59) { //counting minutes for task time
    s = 0;
    m++;
  }

  if (tt_s > 59) { //counting minutes for total time
    tt_s = 0;
    tt_m++;
  }

  // HOURS
  if (tt_m > 59) { //counting hours for total time
    tt_m = 0;
    tt_h++;
  }

  green = read_button(green, GREEN_BUTTON);
  yellow = read_button(yellow, YELLOW_BUTTON);
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
  Serial.print("id: ");Serial.print(id);Serial.print("   av: ");Serial.print(round(data[id].av));Serial.print("   pr: ");Serial.println(data[id].pr);
  //Serial.print("Sum: "); Serial.print(sum); Serial.print("   Laps: "); Serial.println(laps);

  if (data[id].av == 0){ //when average is not defined yet (== 0)
    Serial.print("Aaverage updated: "); Serial.print(data[id].av); Serial.print(" --> "); 
    data[id].av = sum / laps;
  }
  else{
    Serial.print("Average updated: "); Serial.print(data[id].av); Serial.print(" --> "); 
    data[id].av = data[id].av * 0.8 + sum / laps * 0.2 ; //data[id].av = sum / laps; // 124s
    

  }
  Serial.print(data[id].av);
  int av = round(data[id].av);
  Serial.print(" --> "); Serial.println(av);
  //just for display of average
  int av_m = get_min(data[id].av);
  int av_s = get_sec(data[id].av);
  String av_m_ = time_to_string(av_m);
  String av_s_ = time_to_string(av_s);

  tft.fillRect(265,7,80 ,20 ,TFT_BLACK);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
  tft.drawString(av_m_ + ":" + av_s_, 265, 7);

}

bool personal_record(int laptime){

  if (data[id].pr == 0){ //personal record not defined yet
    data[id].pr = laptime; 
    return 1;
  }
  else if (laptime < data[id].pr){ //better time than personal record
    data[id].pr = laptime;
    return 1;
  }
  else 
    return 0;
  /*
  if (pr == 0){ //personal record not defined yet
    pr = laptime; 
    return 1;
  }
  else if (laptime < pr){ //better time than personal record
    pr = laptime;
    return 1;
  }
  else 
    return 0;
    */
}

// displays image and difference between current lap and average of that task
void score_time(){
  // difference between laptime and average time it takes to perform task

  int diff = laptime - round(data[id].av); // 138 - 124 = 14 s (worse) 110 - 124 = -14 s (better)
  int minus = 0;
  if (diff < 0)  minus = 1;
  
  //Serial.print("min: "); Serial.print(min); Serial.print(" sec: "); Serial.println(sec);
  //Serial.print("Laptime: "); Serial.println(laptime);
  //Serial.print("Diff: "); Serial.println(diff); 
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

  if (personal_record(laptime)){
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
  //if (digitalRead(0) == 0) {
  if (green.clicked){
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

      sendInfo_TASK(laptime_m, laptime_s, id);
      tft.fillRect(10, 70, 120, 60, TFT_BLACK);
      s = 0;
      m = 0;
      pom = 1;
    }
  }
    else {
      pom = 0;
    }

  //if (digitalRead(14) == 0) {
  if (yellow.clicked) {
    if (pom2 == 0){
      Serial.print("Sending final info... av: "); Serial.print(round(data[id].av)); Serial.print(" pr: "); Serial.print(data[id].pr); Serial.print(" total h: "); Serial.print(tt_h); Serial.print(" total m: "); Serial.print(tt_m); Serial.print(" total s: "); Serial.println(tt_s);
      sendInfo_final_TASK(id, round(data[id].av), data[id].pr, tt_h, tt_m);
      id = -1;
      waitfortask_screen();
      pom2 = 1;
    }
  }
  else{
    pom2 = 0;
  }
}

