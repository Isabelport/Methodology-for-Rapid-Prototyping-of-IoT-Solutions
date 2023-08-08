#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

//#define implementation

#include "fonts.h"
#include "icons.h"
#include "secrets.h"
#include "rfid.h"
#include "rfid_employee.h"
#include "buttons.h"
#include "communication.h"
#include "tof.h"

//screen pins
#define TFT_CS 6
#define TFT_DC 7
#define TFT_RESET 5

#define color_yellow 0x65DB
bool wifi = 1;


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
int break_m = 0;  //counts the amount of min in break
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
  //Initialize buttons
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  //Initialize screen
  tft.init();
  tft.setRotation(3);  //1 horizontal cabo do lado direito // 3 horizontal cabo do lado esquerdo
  tft.fillScreen(TFT_BLACK);
  delay(100);
  //pinMode(15, OUTPUT); // to boot with battery...
  //digitalWrite(15, 1);  // and/or power from 5v rail instead of USB

#ifndef implementation
  Serial.begin(115200);
#endif
  Serial.println(F("TFT ready"));
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  //ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, 67);
  tft.setSwapBytes(true);

  //Initialize wifi and TB //sendinfo
  if (wifi == 1) {
    tft.drawString("Por favor ligar-se ao Wi-Fi.", 5, 70, 4);
    WiFi.begin(STA_SSID, STA_PASS);
    InitWiFi();
    connectTB();
  }
  tft.fillScreen(TFT_BLACK);

  Serial.println(F("Initialize RFID employee"));
  initRFIDSensor_employee();

  //Initialize RFID
  Serial.println(F("Initialize RFID"));
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);  // CHANGE DEFAULT PINS
  initRFIDSensor();


  //Initialize ToF
  Serial.println(F("Initialize ToF"));
  Wire.setPins(SDA_PIN, SCL_PIN);
  Wire.begin();
  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  initDistSensor();

  assignTaskValues(data);
  Serial.println(F("Ready! Let's start!"));
  sendInfo_task(0, 0, 0);

  waitForCard_screen("employee");
}

//zeros all variables
void reset_screen() {
  //if (first == 1)
  sendInfo_task(0, task_id, emp_id);
  first = 0;
  delay(100);
  tft.fillScreen(TFT_BLACK);
  //tft.init();
  //tft.setRotation(3);  //1 horizontal cabo do lado direito // 3 horizontal cabo do lado esquerdo
  //tft.fillScreen(TFT_BLACK);

  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);

  tft.setTextColor(color_yellow);
  tft.drawString("OPERADOR:", 0, 5);
  tft.setTextColor(TFT_WHITE);
  Serial.println(employee_name[emp_id]);
  tft.drawString(employee_name[emp_id], 95, 5);

  tft.setTextColor(color_yellow);
  tft.drawString("TAREFA:", 0, 22);
  tft.setTextColor(TFT_WHITE);
  Serial.println(data[task_id].task_name);
  tft.drawString(data[task_id].task_name, 67, 22);


  tft.setTextColor(color_yellow);
  tft.drawString("MEDIA:", 210, 22);
  int av = round(data[task_id].av);
  int av_m = getMin(av);
  int av_s = getSec(av);
  String av_m_ = timeToString(av_m);
  String av_s_ = timeToString(av_s);
  tft.setTextColor(TFT_WHITE);
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
  //int pre_task_id = task_id;
  delay(100);
  tft.fillScreen(TFT_BLACK);
  Serial.print("emp_id2: ");
  Serial.println(emp_id);
  //tft.init();
  //tft.setRotation(3);  //1 horizontal cabo do lado direito // 3 horizontal cabo do lado esquerdo
  //tft.fillScreen(TFT_BLACK);

  if (mode == "task") {
    Serial.println(F("Wait for task Screen"));
    tft.setTextColor(color_yellow);
    Serial.println(employee_name[emp_id]);
    tft.drawString(employee_name[emp_id], 5, 40, 4);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Para continuar", 5, 70, 4);
    tft.drawString("inserir cartao da TAREFA.", 5, 95, 4);
    //delay(1000);

    //tft.setTextColor(TFT_CYAN);
    //tft.drawString("ou clicar no botao amarelo", 5, 105, 4);
    //tft.drawString("para trocar de operador", 5, 130, 4);

  } else if (mode == "employee") {
    Serial.println(F("Wait for employee Screen"));
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Para continuar", 5, 50, 4);
    tft.drawString("passar cartao de ID.", 5, 75, 4);
  }

  while (1) {
    int break_s_prev = break_s;

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
                          //
      break_s++;
      Serial.print(F("Break time: "));
      Serial.print(break_m);
      Serial.print(F(":"));
      Serial.println(break_s);
      break_ss = 0;
    }

    // MINUTES
    if (break_s > 59) {  //counting minutes for break time
      break_s = 0;
      break_m++;
    }


    //TOF
    //still checking distance while in break
    if (wifi == 1)
      checkAndSendDistance();




    if (mode == "employee") {
      emp_id = readRFID_employee();
      if (emp_id != -1) {
        waitForCard_screen("task");
        break;
      }
    }

    else if (mode == "task") {
      if (break_s_prev != break_s) { //or else it will crash
        task_id = readRFID();
        if (task_id != -1) {
          break_screen("task");
          break;
        }
      }
    }

    green = readButton(green, GREEN_BUTTON);
    yellow = readButton(yellow, YELLOW_BUTTON);

    if (green.clicked) {
      Serial.println(F("green"));
      if (pom == 0)
        pom = 1;
    } else
      pom = 0;

    if (yellow.clicked) {
      Serial.println(F("yellow"));
      if (mode == "task") {
        Serial.println(F("here"));
        //task_id = -1;
        emp_id = -1;
        waitForCard_screen("employee");
        break;
      }
      if (pom2 == 0)
        pom2 = 1;
    } else
      pom2 = 0;
  }
}

void break_screen(String mode) {
  int prev_emp_id = emp_id;
  int prev_task_id = task_id;
  delay(100);
  tft.fillScreen(TFT_BLACK);

  if (mode == "task") {
    tft.setTextColor(TFT_WHITE);
    Serial.println(F("Break screen task"));
    tft.drawString("Tarefa: ", 5, 40, 4);
    tft.drawString(data[task_id].task_name, 90, 40, 4);

    tft.setTextColor(TFT_GREEN);
    tft.drawString("Clicar no botao verde", 5, 70, 4);
    tft.drawString("para comecar", 5, 95, 4);
  }

  while (1) {
    int prev_break_s = break_s;

    ss1 = millis();

    // MILLISECONDS
    if (ss1 > ss2 + 9) {  //counting milliseconds for break time
      ss2 = ss1;
      break_ss++;
    }

    // SECONDS
    if (break_ss > 99) {  //counting seconds for break time
      Serial.print(F("Break time: "));
      Serial.print(break_m);
      Serial.print(F(":"));
      Serial.println(break_s);
      break_s++;
      break_ss = 0;
    }

    // MINUTES
    if (break_s > 59) {  //counting minutes for break time
      break_s = 0;
      break_m++;
    }

    //TOF
    if (wifi == 1)
      checkAndSendDistance();

    if (prev_break_s != break_s) {  //reading rfid only each second, else it cannot process
      if (mode == "task") {
        task_id = readRFID();

        if (task_id == -1) task_id = prev_task_id;  //mantain id if no card is read
        if ((task_id != -1) && (prev_task_id != task_id)) {
          prev_task_id = task_id;  //update prev_task_id
          break_screen("task");
          break;
        }
      }
    }

    green = readButton(green, GREEN_BUTTON);
    yellow = readButton(yellow, YELLOW_BUTTON);


    if (mode == "employee") {
      if (green.clicked) {
        Serial.println(F("green"));
        if (pom == 0) {
          pom = 1;
          waitForCard_screen("task");
          break;
        }
      } else {
        pom = 0;
      }
      if (yellow.clicked) {
        Serial.println(F("yellow"));
        if (pom2 == 0) {
          //emp_id = -1;
          pom2 = 1;
          //setup();
          //waitForCard_screen("employee");
          //break;
        }
      } else {
        pom2 = 0;
      }
    } else if (mode == "task") {
      if (green.clicked) {
        Serial.println(F("START"));
        Serial.println(F("green"));
        if (wifi == 1)
          sendInfo_task(break_m * 60 + break_s, 0, emp_id);  //tell tb break (7) is over


        if (pom == 0) {
          pom = 1;
        }
        reset_screen();
        break;
      } else {
        pom = 0;
      }
      if (yellow.clicked) {
        Serial.println(F("yellow"));
        if (pom2 == 0) {
          pom2 = 1;
        }
        //task_id = -1;
        emp_id = -1;
        Serial.print("emp_id: ");
        Serial.println(emp_id);
        waitForCard_screen("employee");
        break;
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
    //Serial.println(F(current);
    temp = current;
  }

  if (tt_current != tt_temp) {
    tft.setFreeFont(&DejaVu_LGC_Sans_Bold_14);
    tft.setTextColor(color_yellow);
    tft.drawString("TOTAL:", 5, 150);
    tft.fillRect(70, 150, 50, 15, TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(tt_hours + ":" + tt_minutes, 70, 150);
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
  if (wifi == 1)
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
  Serial.print(F("task_id: "));
  Serial.print(task_id);
  Serial.print(F("   av: "));
  Serial.print(round(data[task_id].av));
  Serial.print(F("   pr: "));
  Serial.println(data[task_id].pr);
  //Serial.print(F("Sum: "); Serial.print(F(sum); Serial.print(F("   Laps: "); Serial.println(F(laps);

  if (data[task_id].av == 0) {  //when average is not defined yet (== 0)
    Serial.print(F("Aaverage updated: "));
    Serial.print(data[task_id].av);
    Serial.print(F(" --> "));
    data[task_id].av = sum / laps;
  } else {
    Serial.print(F("Average updated: "));
    Serial.print(data[task_id].av);
    Serial.print(F(" --> "));
    data[task_id].av = data[task_id].av * 0.8 + sum / laps * 0.2;  //data[task_id].av = sum / laps; // 124s
  }
  Serial.print(data[task_id].av);
  int av = round(data[task_id].av);
  Serial.print(F(" --> "));
  Serial.println(av);
  //just for display of average
  int av_m = getMin(data[task_id].av);
  int av_s = getSec(data[task_id].av);
  String av_m_ = timeToString(av_m);
  String av_s_ = timeToString(av_s);

  tft.fillRect(265, 22, 70, 15, TFT_BLACK);
  tft.setFreeFont(&DejaVu_LGC_Sans_Bold_12);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(av_m_ + ":" + av_s_, 265, 22);
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
    //Serial.println(F("down");
    tft.pushImage(x_image, y_image, 32, 32, down);
    tft.drawString("-" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //INCREASE IN TIME -> UP
  else if (diff > threshold) {  //if difference is bigger than threshold than there is big increase of the laptime in relation to average (worse)
    //Serial.println(F("up");
    tft.pushImage(x_image, y_image, 32, 32, up);
    tft.drawString("+" + diff_m_ + ":" + diff_s_, x_text, y_text, 4);
  }
  //EQUAL
  else {  // if difference is between -threshold and threshold, difference is not considerably big
    //Serial.println(F("equal");
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


      Serial.print(F("Laptime: "));
      Serial.print(laptime_m);
      Serial.print(F(":"));
      Serial.println(laptime_s);

      //lap time is always positive
      if (laptime_m < 0)
        laptime_m = laptime_m * -1;

      if (laptime_s < 0)
        laptime_s = laptime_s * -1;

      scoreTime();
      averageFunc();
      if (wifi == 1)
        sendInfo_task(laptime_m * 60 + laptime_s, task_id, emp_id);
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
      Serial.println(F(" break"));

      if (wifi == 1) {
        sendInfo_final_task(task_id, round(data[task_id].av), data[task_id].pr, tt_h * 60 + tt_m, emp_id);
        sendInfo_task(0, 0, emp_id);
      }
      task_id = -1;
      waitForCard_screen("task");
      pom2 = 1;
    }
  } else {
    pom2 = 0;
  }
}