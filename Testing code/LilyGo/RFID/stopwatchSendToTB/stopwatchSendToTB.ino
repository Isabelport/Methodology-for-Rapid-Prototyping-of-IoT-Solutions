#include <WiFi.h> // WiFi control for ESP32
//#define THINGSBOARD_ENABLE_PROGMEM 0
#include <ThingsBoard.h>  // ThingsBoard SDK
//#include "down.h"
#include "equal.h"
//#include "up.h"
#include "play.h"
#include "stop.h"

#include "secrets.h"

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include "DSEG7_Classic_Regular_50.h"
#include "DSEG7_Classic_Regular_32.h"

#define grey 0x65DB
#define color 0xF251

int ss = 0;  //miliseconds/10
int s = 0;   //seconds
int m = 0;   //minutes

int lss[30];
int ls[30];
int lm[30];

String minutesL;
String secondsL;
String milisL;
int laps = 0;
String lapTime[30];
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

void setup() {
  tft.init();
  tft.setRotation(1); //1 horizontal cabo do lado esquerdo // 3 horizontal cabo do lado direito
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);

  //initialize wifi
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
  reset();
}

String seconds, minutes, milis;
String current, temp;

//int winer = 0;
int av = 0;

long tt = 0;
long tt2 = 0;

void loop() {

  //if (fase < 5) {
    tt = millis();
    //tft.setFreeFont(&DSEG7_Classic_Regular_32);
    if (s < 10) seconds = "0" + String(s);
    else seconds = String(s);
    if (m < 10) minutes = "0" + String(m);
    else minutes = String(m);
    current = minutes + ":" + seconds;


    if (current != temp) {
      tft.setFreeFont(&DSEG7_Classic_Regular_50);
      tft.drawString(current, 10, 60);
      
      //tft.drawString(milis, 60, 76, 7);
      //tft.drawLine(int32_t xs, int32_t ys, int32_t xe, int32_t ye, uint32_t color)
      temp = current;
    }

    if (ss < 10) milis = "0" + String(ss);
    else milis = String(ss);
    //tft.drawString(milis, 60, 76, 7);
  //}

  if (tt > tt2 + 9) {
    tt2 = tt;
    if (fase == 1)
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
  buttons();
}

void sendInfo(int min, int sec, int mil) {
  // Reconnect to WiFi, if needed
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Not connected to WiFi... reconnecting");
    reconnect();
    delay(500);  //return;
  }
  // Reconnect to ThingsBoard, if needed
  while (!tb.connected()) {
    Serial.println("Connecting to The ThingsBoard");
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      delay(500);  //return;
    }
  }
  //Serial.println("-> Sending data...");
  const int data_items = 2;
  Telemetry data[data_items] = {
    { "min", min },
    { "sec", sec },
    };

  tb.sendTelemetry(data, data_items);

  // Process messages
  tb.loop();
}

void buttons() {
  if (digitalRead(43) == 0) {
    if (pom == 0) {
      if (fase == 1) {
        //winer++;
        laps++;
        //if (winer == 1) {
          //tft.fillRect(15, 130, 180, 80, TFT_RED);
          //tft.drawString("LAP:", 20, 130, 2);
        //}
        //if (winer > 5) {
          //tft.fillRect(15, 144, 190, 140, TFT_BLACK); //
        //  winer = 1;
        //}
        //tft.setFreeFont(&DSEG14_Classic_Mini_Regular_15);
        lss[laps] = ss;
        ls[laps] = s;
        lm[laps] = m;
        
        //minutes
        int lm_ = lm[laps] - lm[laps - 1];
        if (lm_ < 0)
          lm_ = lm_ * -1;

        if (lm_ < 10) //
          minutesL = "0" + String(lm_);
        else
          minutesL = String(lm_);

        //seconds
        int ls_ = ls[laps] - ls[laps - 1];
        if (ls_ < 0)
          ls_ = ls_ * -1;

        if (ls_ < 10)        
          secondsL = "0" + String(ls_);
        else
          secondsL = String(ls_);

        //milisseconds
        int lss_ = lss[laps] - lss[laps - 1];
        if (lss_ < 0)
          lss_ = lss_ * -1;

        if (lss_ < 10)
          milisL = "0" + String(lss_);
        else
          milisL = String(lss_);


        sendInfo(lm_, ls_, lss_);
        lapTime[laps - 1] = minutesL + " : " + secondsL + " : " + milisL;
        //tft.drawString(lapTime[laps - 1], 18, 130 + winer * 22);
      }

      if (fase == 0)
        fase = 1;
      pom = 1;
    }
  } else {
    pom = 0;
  }

  if (digitalRead(44) == 0) {
    if (pom2 == 0) {


    tft.fillScreen(TFT_BLACK);
    //tft.setFreeFont(&DSEG14_Classic_Mini_Regular_15);
    tft.drawString("TIME:", 10, 6, 2);
    tft.drawString("LAP:", 10, 50, 2);
    tft.drawString(current + ":" + milis, 10, 28);
    for (int i = 0; i < laps; i++) {

      tft.drawString(String(i + 1) + ".   " + lapTime[i], 10, 68 + (14 * i), 2);
    }

      pom2 = 1;
    }
  } else {
    pom2 = 0;
  }
}
void reset() {
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(240, 60, 50, 50, equal);
  //tft.pushImage(30,136,72,72,play);
  tft.drawString("ACTIVITY", 14, 7, 2);

  //tft.fillRect(105,224,4,12,color);
  //tft.setFreeFont(&DSEG7_Classic_Regular_32);
  tft.drawLine(200,0,200,200,grey);
  //tft.drawLine(int32_t xs, int32_t ys, int32_t xe, int32_t ye, uint32_t color)

  laps = 0;
  lss[0] = 0;
  ls[0] = 0;
  lm[0] = 0;
  ss = 0;
  s = 0;
  m = 0;
  fase = 0;
  //winer = 0;
}
