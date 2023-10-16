#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include "fonts.h"
#include "icons.h"
#include "secrets.h"
#include "rfid.h"
#include "buttons.h"
#include "communication.h"
//#include "laptime.h"
#include "tof.h"

//screen pins
#define TFT_CS 6
#define TFT_DC 7
#define TFT_RESET 5

#define color_yellow 0x65DB

#define STATION 1  //1 or 2

//Time related
int laps = 0;
int sum = 0;
long ss1 = 0;  //aux to count milliseconds
long ss2 = 0;

int ss = 0;               //miliseconds
int s = 0;                //seconds
int m = 0;                //minutes
String seconds, minutes;  //minutes and seconds of current task
String current, temp;     //current time and auxiliar variable
int tt_h = 0;             //total hours and minutes of certain task
int tt_m = 0;
int tt_s = 0;
int tt_ss = 0;
String tt_hours, tt_minutes, tt_seconds;
String tt_current, tt_temp;
int laptime = 0;  //laptime in seconds
int laptime_m = 0;
int laptime_s = 0;
int break_m = 0;  //counts the amount of time in break
int break_s = 2;  //accounts for the delay it takes to start
int break_ss = 0;
//Interface
int first = 1;

//CHANGE ACCORDING TO TASK
//time of difference between an increase or decrease in relation to average of time of certain task
// if threshold = 1, if average is 6, if you score 7 it is an increase, 6 equal and 5 decrease
// if threshold = 2, if average is 6, if you score 7 is considered equal, 8 is increase, 6 is equal, 5 is equal, 4 is decrease
int threshold = 1;


/*******************************************CODE*********************************************/
void setup() {
  //Initialize screen
  tft.init();
  tft.setRotation(3);  //1 horizontal cabo do lado direito // 3 horizontal cabo do lado esquerdo
  tft.fillScreen(TFT_BLACK);
  Serial.begin(115200);
  Serial.println("TFT ready");
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  //ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
  tft.setSwapBytes(true);

  //Initialize buttons
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);

  //Initialize wifi and TB
  tft.drawString("Por favor ligar-se ao Wi-Fi.", 5, 70, 4);
  WiFi.begin(STA_SSID, STA_PASS);
  InitWiFi();
  connectTB();
  tft.fillScreen(TFT_BLACK);

  //Initialize RFID
  Serial.println("Initialize RFID");
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);  // CHANGE DEFAULT PINS
  initRFIDSensor();

  //Initialize ToF
  Serial.println("Initialize ToF");
  Wire.setPins(SDA_PIN, SCL_PIN);
  Wire.begin();
  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  initDistSensor();

  assignTaskValues(data);
  Serial.println("Ready! Let's start!");

  waitForCard_screen("employee");
}

//zeros all variables
void reset() {
  first = 0;
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);

  tft.setTextColor(color_yellow);
  tft.drawString("OPERADOR:", 0, 5);
  tft.drawString(employee_name[emp_id], 90, 5);

  tft.setTextColor(TFT_WHITE);
  tft.drawString("TAREFA:", 0, 22);
  tft.drawString(data[task_id].task_name, 67, 22);

  tft.drawString("MEDIA:", 210, 22);
  int av = round(data[task_id].av);
  int av_m = getMin(av);
  int av_s = getSec(av);
  String av_m_ = timeToString(av_m);
  String av_s_ = timeToString(av_s);
  if (av != 0)
    tft.drawString(av_m_ + ":" + av_s_, 265, 22);

  tft.drawLine(200, 42, 200, 200, color_yellow);
  tft.drawLine(0, 42, 500, 42, color_yellow);

  ss = 0;
  s = 0;
  m = 0;
  laps = 0;
  data[task_id].pr = 0;
  tt_ss = 0;
  tt_s = 0;
  tt_m = 0;
  tt_h = 0;
  sum = 0;
  break_m = 0;
  break_s = 2;
  break_ss = 0;
}

void waitForCard_screen(String mode) {
  tft.fillScreen(TFT_BLACK);

  if (mode == "task") {
    Serial.println("Wait for task Screen");

    tft.setTextColor(TFT_WHITE);
    tft.drawString(employee_name[emp_id], 5, 45, 4);
    tft.drawString("Para comecar", 5, 70, 4);
    tft.drawString("passar cartao da tarefa.", 5, 95, 4);



  } else {
    Serial.println("Wait for employee Screen");
    if (first == 0) {
      tft.setTextColor(TFT_WHITE);
      tft.drawString("Pausa", 5, 45, 4);
      tft.drawString("Para continuar", 5, 70, 4);
      tft.drawString("passar cartao de identific.", 5, 95, 4);
    } else {
      tft.setTextColor(TFT_WHITE);
      tft.drawString("Para continuar", 5, 60, 4);
      tft.drawString("passar cartao de identific.", 5, 85, 4);
    }
  }

  while (1) {

    if (first == 0) {
      ss1 = millis();

      // MILLISECONDS
      if (ss1 > ss2 + 9) {  //counting milliseconds for break time
        ss2 = ss1;
        break_ss++;
      }

      // SECONDS
      //since the function of readrfid takes a part of the processing, the seconds seem "slower",
      //so we assumed that 40ms are 1 second, which in the end actually look like one second
      if (break_ss > 40) {  //counting "seconds" for break time
        Serial.print("Break time: ");
        Serial.print(break_m);
        Serial.print(":");
        Serial.println(break_s);
        break_s++;
        break_ss = 0;
      }

      // MINUTES
      if (break_s > 59) {  //counting minutes for break time
        break_s = 0;
        break_m++;
      }
    }

    //TOF
    //still checking distance while in break
    checkAndSendDistance();

    if (mode == "task") {
      if (STATION == 1)
        task_id = readRFID("task_stat1");
      else
        task_id = readRFID("task_stat2");
      if (task_id != -1) {
        break_screen("task");
        break;
      }
    } else {
      emp_id = readRFID("employee");
      //emp_id--;
      if (emp_id != -1) {
        break_screen("employee");
        break;
      }
    }
    //DELETE BUTTONS??

    green = readButton(green, GREEN_BUTTON);
    yellow = readButton(yellow, YELLOW_BUTTON);
    if (green.clicked) {
      Serial.println("green");
      if (pom == 0)
        pom = 1;
    } else
      pom = 0;

    if (yellow.clicked) {
      Serial.println("yellow");
      if (pom2 == 0)
        pom2 = 1;
      if (mode == "task") {
        waitForCard_screen("employee");
        break;
      }
    } else
      pom2 = 0;
  }
}

void break_screen(String mode) {

  Serial.println("hello");
  tft.fillScreen(TFT_BLACK);

  if (mode == "employee") {

    tft.setTextColor(TFT_WHITE);
    Serial.println("Break screen employee");
    Serial.println(employee_name[emp_id]);
    tft.drawString("Operador: ", 5, 20, 4);
    tft.drawString(employee_name[emp_id], 125, 20, 4);

    tft.setTextColor(TFT_CYAN);
    tft.drawString("Clicar no botao amarelo", 5, 50, 4);
    tft.drawString("para trocar de operador", 5, 75, 4);

    tft.setTextColor(TFT_GREEN);
    tft.drawString("ou no botao verde", 5, 105, 4);
    tft.drawString("para continuar", 5, 130, 4);

  } else {
    tft.setTextColor(TFT_WHITE);
    Serial.println("Break screen task");
    tft.drawString("Tarefa: ", 5, 20, 4);
    tft.drawString(data[task_id].task_name, 90, 20, 4);

    tft.setTextColor(TFT_CYAN);
    tft.drawString("Clicar no botao amarelo", 5, 50, 4);
    tft.drawString("para trocar de tarefa", 5, 75, 4);

    tft.setTextColor(TFT_GREEN);
    tft.drawString("ou no botao verde", 5, 105, 4);
    tft.drawString("para comecar", 5, 130, 4);
  }

  while (1) {
    if (first == 0) {
      ss1 = millis();

      // MILLISECONDS
      if (ss1 > ss2 + 9) {  //counting milliseconds for break time
        ss2 = ss1;
        break_ss++;
      }

      // SECONDS
      if (break_ss > 99) {  //counting seconds for break time
        Serial.print("Break time: ");
        Serial.print(break_m);
        Serial.print(":");
        Serial.println(break_s);
        break_s++;
        break_ss = 0;
      }

      // MINUTES
      if (break_s > 59) {  //counting minutes for break time
        break_s = 0;
        break_m++;
      }
    }
    //TOF
    checkAndSendDistance();

    green = readButton(green, GREEN_BUTTON);
    yellow = readButton(yellow, YELLOW_BUTTON);

    if (mode == "employee") {
      if (green.clicked) {
        Serial.println("green");
        if (pom == 0) {
          pom = 1;
          waitForCard_screen("task");
          break;
        }
      } else {
        pom = 0;
      }
      if (yellow.clicked) {
        Serial.println("yellow");
        if (pom2 == 0) {
          emp_id = -1;
          pom2 = 1;
          waitForCard_screen("employee");
          break;
        }
      } else {
        pom2 = 0;
      }
    } else {
      if (green.clicked) {
        if (first == 0) {
          sendInfo_task(break_m, break_s, 0);  //tell tb break is over
        }
        Serial.println("green");
        if (pom == 0) {
          pom = 1;
          reset();
          break;
        }
      } else {
        pom = 0;
      }
      if (yellow.clicked) {
        Serial.println("yellow");
        if (pom2 == 0) {
          task_id = -1;
          pom2 = 1;
          waitForCard_screen("task");
          break;
        }
      } else {
        pom2 = 0;
      }
    }
  }
}


void loop() {
  ss1 = millis();

  minutes = timeToString(m);
  seconds = timeToString(s);

  tt_hours = timeToString(tt_h);
  tt_minutes = timeToString(tt_m);
  tt_seconds = timeToString(tt_s);

  current = minutes + ":" + seconds;
  tt_current = tt_hours + ":" + tt_minutes + ":" + tt_seconds;
  if (current != temp) {
    tft.setFreeFont(&DSEG7_Classic_Regular_50);
    tft.setTextColor(TFT_WHITE);
    tft.fillRect(5, 70, 180, 60, TFT_BLACK);
    tft.drawString(current, 5, 70);
    //Serial.println(current);
    temp = current;
  }

  if (tt_current != tt_temp) {
    tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
    tft.drawString("TOTAL:", 5, 130);
    tft.fillRect(70, 130, 50, 15, TFT_BLACK);
    tft.drawString(tt_hours + ":" + tt_minutes, 70, 130);
    tt_temp = tt_current;
  }

  // MILLISECONDS
  if (ss1 > ss2 + 9) {  //counting milliseconds
    ss2 = ss1;
    ss++;
    tt_ss++;
  }

  // SECONDS
  if (ss > 99) {  //counting seconds for task time
    s++;
    ss = 0;
  }

  if (tt_ss > 99) {  //counting seconds for total time

    tt_s++;
    tt_ss = 0;
  }

  // MINUTES
  if (s > 59) {  //counting minutes for task time
    s = 0;
    m++;
  }

  if (tt_s > 59) {  //counting minutes for total time
    tt_s = 0;
    tt_m++;
  }

  // HOURS
  if (tt_m > 59) {  //counting hours for total time
    tt_m = 0;
    tt_h++;
  }

  //buttons
  green = readButton(green, GREEN_BUTTON);
  yellow = readButton(yellow, YELLOW_BUTTON);
  buttons();

  //TOF
  checkAndSendDistance();
}

String timeToString(int t) {
  String ts;
  if (t < 10) ts = "0" + String(t);
  else ts = String(t);
  return ts;
}

int getMin(int total) {
  int min = total / 60;
  return min;
}

int getSec(int total) {
  int sec = total % 60;
  return sec;
}

void averageFunc() {
  sum = sum + laptime;
  float av_float;
  Serial.print("task_id: ");
  Serial.print(task_id);
  Serial.print("   av: ");
  Serial.print(round(data[task_id].av));
  Serial.print("   pr: ");
  Serial.println(data[task_id].pr);
  //Serial.print("Sum: "); Serial.print(sum); Serial.print("   Laps: "); Serial.println(laps);

  if (data[task_id].av == 0) {  //when average is not defined yet (== 0)
    Serial.print("Aaverage updated: ");
    Serial.print(data[task_id].av);
    Serial.print(" --> ");
    data[task_id].av = sum / laps;
  } else {
    Serial.print("Average updated: ");
    Serial.print(data[task_id].av);
    Serial.print(" --> ");
    data[task_id].av = data[task_id].av * 0.8 + sum / laps * 0.2;  //data[task_id].av = sum / laps; // 124s
  }
  Serial.print(data[task_id].av);
  int av = round(data[task_id].av);
  Serial.print(" --> ");
  Serial.println(av);
  //just for display of average
  int av_m = getMin(data[task_id].av);
  int av_s = getSec(data[task_id].av);
  String av_m_ = timeToString(av_m);
  String av_s_ = timeToString(av_s);

  tft.fillRect(265, 5, 50, 15, TFT_BLACK);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
  tft.drawString(av_m_ + ":" + av_s_, 265, 25);
}

bool personal_record(int laptime) {

  if (data[task_id].pr == 0) {  //personal record not defined yet
    data[task_id].pr = data[task_id].av;
  }
  if (laptime < data[task_id].pr) {  //better time than personal record
    data[task_id].pr = laptime;
    return 1;
  } else
    return 0;
}

// displays image and difference between current lap and average of that task
void scoreTime() {
  // difference between laptime and average time it takes to perform task

  int diff = laptime - round(data[task_id].av);  // 138 - 124 = 14 s (worse) 110 - 124 = -14 s (better)
  int minus = 0;
  if (diff < 0) minus = 1;

  int diff_m = getMin(diff);
  int diff_s = getSec(diff);

  if (diff_s < 0)
    diff_s = -diff_s;
  if (diff_m < 0)
    diff_m = -diff_m;

  String diff_m_ = timeToString(diff_m);
  String diff_s_ = timeToString(diff_s);

  int x_image = 250;
  int y_image = 72;
  int x_text = 225;
  int y_text = 112;

  tft.fillRect(x_text, y_text, 80, 100, TFT_BLACK);
  //PERSONAL RECORD
  if (personal_record(laptime)) {
    tft.pushImage(x_image, y_image, 32, 32, star);
    if (minus) tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
    else tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //DECREASE IN TIME -> DOWN
  else if (diff < -threshold) {  //if difference is less than -threshold than there is big decrease of the laptime in relation to average (better)
    //Serial.println("down");
    tft.pushImage(x_image, y_image, 32, 32, down);
    tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //INCREASE IN TIME -> UP
  else if (diff > threshold) {  //if difference is bigger than threshold than there is big increase of the laptime in relation to average (worse)
    //Serial.println("up");
    tft.pushImage(x_image, y_image, 32, 32, up);
    tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //EQUAL
  else {  // if difference is between -threshold and threshold, difference is not considerably big
    //Serial.println("equal");
    tft.pushImage(x_image, y_image, 32, 32, equal);  //32 32
    if (minus) tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
    else tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
}

void buttons() {
  if (green.clicked) {
    if (pom == 0) {
      laps++;

      laptime_m = m;
      laptime_s = s;
      laptime = laptime_m * 60 + laptime_s;


      Serial.print("Laptime: ");
      Serial.print(laptime_m);
      Serial.print(":");
      Serial.println(laptime_s);

      //lap time is always positive
      if (laptime_m < 0)
        laptime_m = laptime_m * -1;

      if (laptime_s < 0)
        laptime_s = laptime_s * -1;

      scoreTime();
      averageFunc();

      sendInfo_task(laptime_m, laptime_s, task_id);
      tft.fillRect(10, 70, 120, 60, TFT_BLACK);
      s = 0;
      m = 0;
      pom = 1;
    }
  } else {
    pom = 0;
  }

  if (yellow.clicked) {
    if (pom2 == 0) {
      Serial.print("Sending final info... av: ");
      Serial.print(round(data[task_id].av));
      Serial.print(" pr: ");
      Serial.print(data[task_id].pr);
      Serial.print(" taskid: ");
      Serial.print(task_id);
      Serial.print(" total h: ");
      Serial.print(tt_h);
      Serial.print(" total m: ");
      Serial.print(tt_m);
      Serial.print(" break");

      sendInfo_final_task(task_id, round(data[task_id].av), data[task_id].pr, tt_h, tt_m);
      task_id = -1;
      waitForCard_screen("task");
      pom2 = 1;
    }
  } else {
    pom2 = 0;
  }
}
