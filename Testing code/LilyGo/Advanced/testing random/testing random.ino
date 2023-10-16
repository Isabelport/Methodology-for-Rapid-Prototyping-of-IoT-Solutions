#include "pitches.h"

unsigned long mil = 0;
#define BUZZER_PIN 18
int first=1;
int prevTask = -1;


void setup() {
  // initialize digital pin GIOP18 as an output.
  Serial.begin(115200);
  delay(5000);
  
}

// the loop function runs over and over again forever
void loop() {

  // between 0min and 0ssec and 14min and 15 sec
  int numberOfTasks = 0;
  int taskId = random(6);  // 0 -> break, 1-4 -> tasks
  //int prevTask = taskId

  if (prevTask != taskId) {
    tone(BUZZER_PIN, NOTE_C4, 0.1);
    delay(500);
    noTone(BUZZER_PIN);
    tone(BUZZER_PIN, NOTE_C4, 0.1);
    delay(500);
    noTone(BUZZER_PIN);
    //delay(4000);
  }

  if (taskId != 0) {               //not break
    numberOfTasks = random(3, 6);  //max 4 tasks of certain taskid
    while (numberOfTasks != 0) {
      int randMin = random(0);   //3 //0 to
      int randSec = random(10, 20);  //60 //0 to 59
      int totalMil = (randMin * 60 + randSec) * 1000;
      Serial.print("x");
      Serial.println(numberOfTasks);
      Serial.print("TASK: ");
      Serial.println(taskId);
      Serial.print("Time: ");
      Serial.print(randMin);
      Serial.print(":");
      Serial.println(randSec);
      Serial.println();
      //delay(1000);
      buzzbuzz();
      delay(totalMil - 500);
      numberOfTasks = numberOfTasks - 1;
    }
  }
  if (taskId == 0) {           //break
    int randMin = random(2);  //10 //0 and 9
    int randSec = random(1, 60);
    int totalMil = (randMin * 60 + randSec) * 1000;
    Serial.println("Break");
    Serial.print("Time: ");
    Serial.print(randMin);
    Serial.print(":");
    Serial.println(randSec);
    Serial.println();
    //delay(1000);
    buzzbuzz();
    delay(totalMil - 500);  //minus 10 seconds
  }
  prevTask = taskId;
}

void buzzbuzz() {
  tone(BUZZER_PIN, NOTE_C4, 0.2);
  delay(100);
  noTone(BUZZER_PIN);
}
