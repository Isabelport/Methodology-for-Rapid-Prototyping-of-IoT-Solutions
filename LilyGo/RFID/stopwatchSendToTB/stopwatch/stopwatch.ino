#include "stop.h"
#include "play.h"
#include "up.h"

#include "equal.h"
#include "down.h"

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include "DSEG7_Classic_Regular_16.h"
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

void setup() {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);


  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
  reset();
}

String seconds, minutes, milis;
String current, temp;

int winer = 0;

long tt = 0;
long tt2 = 0;

void loop() {

  if (fase < 5) {
    tt = millis();
    tft.setFreeFont(&DSEG7_Classic_Regular_32);
    if (s < 10) seconds = "0" + String(s);
    else seconds = String(s);
    if (m < 10) minutes = "0" + String(m);
    else minutes = String(m);
    current = minutes + ":" + seconds;


    if (current != temp) {
      tft.drawString(current, 10, 30);
      temp = current;
    }

    if (ss < 10) milis = "0" + String(ss);
    else milis = String(ss);
    tft.drawString(milis, 60, 76, 7);
  }

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

void buttons() {
  if (digitalRead(44) == 0) {
    if (pom == 0) {
      if (fase == 1) {
        winer++;
        laps++;
        if (winer == 1) {
          //tft.fillRect(15, 130, 180, 80, TFT_RED);
          tft.drawString("LAP:", 20, 130, 2);
        }
        if (winer > 5) {
          tft.fillRect(15, 144, 190, 140, TFT_BLACK); //
          winer = 1;
        }
        tft.setFreeFont(&DSEG14_Classic_Mini_Regular_15);
        lss[laps] = ss;
        ls[laps] = s;
        lm[laps] = m;
        
        if ((lm[laps] - lm[laps - 1]) < 10) //
          minutesL = "0" + String(lm[laps] - lm[laps - 1]);
        else
          minutesL = String(lm[laps] - lm[laps - 1]);

        if ((ls[laps] - ls[laps - 1]) < 10)
          secondsL = "0" + String(ls[laps] - ls[laps - 1]);
        else
          secondsL = String(ls[laps] - ls[laps - 1]);

        int lsss = lss[laps] - lss[laps - 1];
        if (lsss < 0)
          lsss = lsss * -1;

        if (lsss < 10)
          milisL = "0" + String(lsss);
        else
          milisL = String(lsss);

        lapTime[laps - 1] = minutesL + " : " + secondsL + " : " + milisL;
        tft.drawString(lapTime[laps - 1], 18, 130 + winer * 22);
      }

      if (fase == 0)
        fase = 1;
      pom = 1;
    }
  } else {
    pom = 0;
  }

  if (digitalRead(43) == 0) {
    if (pom2 == 0) {
      fase++;
      if (fase == 5) {

        tft.fillScreen(TFT_BLACK);
        tft.setFreeFont(&DSEG14_Classic_Mini_Regular_15);
        tft.drawString("TIME:", 10, 6, 2);
        tft.drawString("LAP:", 10, 50, 2);
        tft.drawString(current + ":" + milis, 10, 28);
        for (int i = 0; i < laps; i++) {

          tft.drawString(String(i + 1) + ".   " + lapTime[i], 10, 68 + (14 * i), 2);
        }
      }
      if (fase > 5) {

        reset();
      }

      pom2 = 1;
    }
  } else {
    pom2 = 0;
  }
}
void reset() {
  tft.fillScreen(TFT_BLACK);
  //tft.setSwapBytes(true);
  tft.pushImage(0, 0, 48, 48, play);
  tft.pushImage(50,50,50,50,stop);
  tft.pushImage(100,100,50,50,up);

  //tft.pushImage(30,136,72,72,play);
  tft.drawString("STOPWATCH", 14, 7, 2);

  //tft.fillRect(105,224,4,12,color);
  tft.setFreeFont(&DSEG7_Classic_Regular_32);
  //tft.drawLine(0,220,128,220,grey);
  laps = 0;
  lss[0] = 0;
  ls[0] = 0;
  lm[0] = 0;
  ss = 0;
  s = 0;
  m = 0;
  fase = 0;
  winer = 0;
}
