
#include <WiFi.h> // WiFi control for ESP32
//#define THINGSBOARD_ENABLE_PROGMEM 0
#include <ThingsBoard.h>  // ThingsBoard SDK
//#include "down.h"
//#include "equal.h"
//#include "up.h"
//#include "play.h"
//#include "stop.h"

#include "secrets.h"

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include "DSEG7_Classic_Regular_50.h"
#include "DSEG7_Classic_Regular_32.h"
#include "DejaVu_LGC_Sans_Bold_12.h"

// Include the header files that contain the icons
#include "Down.h"
#include "Equal.h"
#include "Up.h"
#include "Rainbow.h"

#define yellow 0x65DB
#define color 0xF251
#define BETTER 2
#define SAME 1
#define WORSE 0

int ss = 0;  //miliseconds/10
int s = 0;   //seconds
int m = 0;   //minutes

int ls[2];
int lm[2];

String minutesL;
String secondsL;
int laps = 0;

int sum = 0;

//value given by rfid
int id = 2;

struct task {
  String task_name;
  int av;
  int pr;
};

task data[4]; //4 structs of task

int av[4] = {0,0,0,0};
/*
int av_m[4] = {0,0,0,0};
int av_s[4] = {0,0,0,0};
String avS[4] = {av_m[0] + ":" av_s[0], av_m[1] + ":" av_s[1], av_m[2] + ":" av_s[2], av_m[3] + ":" av_s[3]};*/

String score; //will give the "score" of the task done, ex: +2:30 or -4:24 


// CHANGE ACCORIDNG TO TASK
int treshold = 1; //time of difference between an increase or decrease in relation to average of time of certain task 

String lapTime[2];
int pom = 0;  //debouncing button
int pom2 = 0;

int fase = 0;
int c = 0;

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;


void setup() {
  tft.init();
  tft.setRotation(1); //1 horizontal cabo do lado esquerdo // 3 horizontal cabo do lado direito
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);

  tft.drawString("Por favor ligar-se ao Wi-Fi.", 14, 70, 4);

  //initialize wifi
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();
  connectTB();



  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  //ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
  reset(id);
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
  /*
  while (!tb.connected()) {
    Serial.println("Connecting to The ThingsBoard");
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      delay(500);  //return;
    }
  }*/
  //Serial.println("-> Sending data...");
  const int data_items = 3;
  Telemetry data[data_items] = {
    { "min", min },
    { "sec", sec },
    { "task", task },
    };

  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}


void reset(int id) {
  tft.fillScreen(TFT_BLACK);
  //tft.setSwapBytes(true);
  //tft.pushImage(240, 60, 20, 20, equal);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);

  assign_values(id);

  tft.drawString("TAREFA:", 10, 7);
  Serial.println(data[id].task_name);
  tft.drawString(data[id].task_name, 75, 7);

  tft.drawString("MEDIA:", 210, 7);
  Serial.println(data[id].av);
  int av_m = get_min(data[id].av);
  int av_s = get_sec(data[id].av);
  String av_m_ = time_to_string(av_m);
  String av_s_ = time_to_string(av_s);
  Serial.println(av_m_ + ":" + av_s_);
  tft.drawString(av_m_ + ":" + av_s_, 265, 7);

  //tft.pushImage (100, 50, 95, 39, rainbow); 


  tft.drawLine(200,30,200,200,yellow);
  tft.drawLine(0,30,500,30,yellow);

  laps = 0;
  ls[0] = 0;
  ls[1] = 0;
  lm[0] = 0;
  lm[0] = 0;
  s = 0;
  m = 0;
}

void break_screen(){
  Serial.println("break");
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Pausa", 14, 50, 4);
  tft.drawString("Clicar no botao de baixo", 14, 70, 4);
  tft.drawString("para recomecar", 14, 90, 4);
  while(1){
    if (digitalRead(0) == 0){
      if (pom == 0){
        //reset(id);
        pom = 1;
        break;
      }
      }
      else {
      pom = 0;
      //reset(id);
      //break;
      }
  }
  //delay(1000);
  id=3;
  reset(id);
  Serial.println("go to loop");
  loop();
}


String seconds, minutes;
String current, temp;

//int winer = 0;

long tt = 0;
long tt2 = 0;

void loop() {

    tt = millis();
    seconds = time_to_string(s);
    minutes = time_to_string(m);

    current = minutes + ":" + seconds;


    if (current != temp) {
      tft.setFreeFont(&DSEG7_Classic_Regular_50);
      tft.drawString(current, 10, 70);

      temp = current;
    }


  if (tt > tt2 + 9) {
    tt2 = tt;
    ss++;
  }

  if (ss > 99) {
    s++;
    ss = 0;
  }
  if (s > 59) {
    s = 0;
    m++;
  }

  buttons(id);
}

String time_to_string(int t){
  String ts;
  if (t < 10) ts = "0" + String(t);
  else ts = String(t);
  return ts;
}

void assign_values(int id){ // id 0-4

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

  return;
}

int get_min(int total){
  int min = total / 60;
  return min;  
}

int get_sec(int total){
  int sec = total % 60;
  return sec;
}

void average_func(int sec, int min, int laps, int id){
  sum = sum + (sec + min * 60);
  
  if (data[id].av == sum / laps) return; //average stays the same
  Serial.print("sum: "); Serial.println(sum);
  Serial.print("laps: "); Serial.println(laps);
  data[id].av = (data[id].av + sum / laps) / 2; //data[id].av = sum / laps; // 124s
  
  int av_m = get_min(data[id].av);
  int av_s = get_sec(data[id].av);
  String av_m_ = time_to_string(av_m);
  String av_s_ = time_to_string(av_s);

  Serial.print("Average updated: ");
  Serial.println(av_m_ + ":" + av_s_);
  tft.fillRect(265,7,80 ,20 ,TFT_BLACK);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
  tft.drawString(av_m_ + ":" + av_s_, 265, 7);

}

// displays image and difference between current lap and average of that task
void score_time(int sec, int min, int id){
  // difference between laptime and average time it takes to perform task
  int diff = (sec + min * 60) - data[id].av; // 138 - 124 = 14 s (worse) 110 - 124 = -14 s (better)
  int minus = 0;
  if (diff < 0)  minus = 1;

  Serial.print("av time: ");  Serial.println(data[id].av);
  Serial.print("min: "); Serial.print(min); Serial.print(" sec: "); Serial.println(sec);
  Serial.print("diff: "); Serial.println(diff);
  int diff_m = get_min(diff);
  int diff_s = get_sec(diff);
  Serial.print("m of laptime: ");  Serial.println(diff_m);
  Serial.print("s of laptime: ");  Serial.println(diff_s);
  if (diff_s < 0 )
    diff_s = - diff_s;
  if (diff_m < 0 )
    diff_m = - diff_m; 

  String diff_m_ = time_to_string(diff_m);
  String diff_s_ = time_to_string(diff_s);

  Serial.println(diff_m_ + ":" + diff_s_);

  int x_image = 245;
  int y_image = 65;
  int x_text = 220;
  int y_text = 105;

  //DECREASE IN TIME -> DOWN
  if (diff < - treshold){ //if difference is less than -2 than there is big decrease of the laptime in relation to average (better)
    Serial.println("down");  
    tft.pushImage (x_image, y_image, 32, 32, down); 
    tft.fillRect( x_text, y_text, 35, 90, TFT_BLACK);
    tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //INCREASE IN TIME -> UP
  else if(diff > treshold){ //if difference is bigger than 2 than there is big increase of the laptime in relation to average (worse)
    Serial.println("up");
    tft.pushImage (x_image, y_image, 32, 32, up); 
    tft.fillRect( x_text, y_text, 35, 90, TFT_BLACK);
    tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //EQUAL
  else{ // if difference is between -2 and 2, difference is not considerably big 
    Serial.println("equal");
    tft.pushImage (x_image, y_image, 32, 32, equal); //32 32
    tft.fillRect( x_text, y_text, 35, 90, TFT_BLACK);
    if (minus) tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
    else tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
    
  }
}



void buttons(int id) {
  if (digitalRead(0) == 0) {
    if (pom == 0) {
      laps++;
      //task data[id] = {assign_values(id)}; // mock function to assign values of task
      //reset(data[id].task_name, data[id].av);
      //average_func(s,m, id); // updates average of certain task
      
      if (laps == 1){ // [ 0 ; s ] [ 0 ; m ]
        ls[1] = s;
        lm[1] = m;
      }
      else if (laps == 2){ // [ s ; s(old) ] [ m ; m(old) ]
        ls[0] = s;
        lm[0] = m;
      }
      else{ // [ s ; s(old) ] [ m ; m(old) ]
        ls[1] = ls[0];
        lm[1] = lm[0];
        ls[0] = s;
        lm[0] = m;
      }
      Serial.print("ls[0] ls[1] e lm[0] lm[1]: "); Serial.print(ls[0]); Serial.print(" "); Serial.print(ls[1]); Serial.print(" "); Serial.print(lm[0]); Serial.print(" "); Serial.println(lm[1]); 
      
      int laptime = (lm[1]*60 - lm[0]*60) + (ls[1] - ls[0]);
      int laptime_m = get_min(laptime);
      int laptime_s = get_sec(laptime);
      //int lm_ = lm[1] - lm[0];
      //int ls_ = ls[1] - ls[0];

      //lap time is always positive
      if (laptime_m < 0)
        laptime_m = laptime_m * -1;
        
      if (laptime_s < 0)
        laptime_s = laptime_s * -1;

      score_time(laptime_s,laptime_m,id); // , -1
  	  //average_func(laptime_s, laptime_m, laps, id);

      //minutesL = time_to_string(lm_);
      //secondsL = time_to_string(ls_);

      sendInfo(laptime_m, laptime_s, id);
      
      pom = 1;
    }
  }
    else {
      pom = 0;
    }

  if (digitalRead(14) == 0) {
    if (pom2 == 0){
      Serial.println("button2");
      break_screen();
    
      pom2 = 1;
    }
  }
  else{
    pom2 = 0;
  }
}