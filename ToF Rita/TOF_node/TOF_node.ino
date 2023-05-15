#include <Arduino.h>
#include <Wire.h>

#include <CayenneLPP.h>
CayenneLPP lpp(44);

volatile bool sleep_flag = true;

#define TRUE 1
#define FALSE 0
// END FLASH

#include <Sodaq_wdt.h>
#include <RTCZero.h> //timer thing
RTCZero rtc; //Create an rtc object 
#include <FlashStorage.h> //to store the sequence number
#include <avr/dtostrf.h>

#define LUX_ANALOG A1
#define LED_R A0
#define LED_B A3
#define LUX_ENABLE 7
#define MEDIR_CAP 3 //PA09
#define CAP_V A2
#define RFM_CS 11 //PA07
#define RFM_DIO0 5 //PA17
#define RFM_RST 2//PA15

//const byte voltage_interrupt = A3;
#define Serial Serial

//END DEFINITION ; END SCREEN DEFINE
//Flash Storage
FlashStorage(seq_0, byte);
FlashStorage(seq_1, byte);
FlashStorage(DST, int); //0 not written, 1 none, 2 DST

#define CONSOLE_STREAM   Serial
#define MODEM_STREAM     Serial1
#define HASH_SIZE 32
#define BLOCK_SIZE 64

int count = 0;

bool initmodem = false;
volatile bool isClock = false; //variables shared between main code and ISR must be declared volatile to tell the compiler that their variable can change at any time
volatile bool isButton = false;

void alarmMatch(){
  isClock = true; 
}

int reset_cause = 0; //0 - normal power on ; 1 - Reset requested by System ; 2 - Reset by Watchdog ; 3 - External reset requested ; 4 - Reset Brown Out 3.3V; 5 - Reset Brown Out 1.2V
bool first_keepalive = true; //just send reset cause on first keepalive
//#define REG_PM_RCAUSE (0x40000438U)

//COMMUNICATION STUFF
#define SERIAL_DEBUG Serial  // serial USB debug - Arm M0
#define SERIAL_MODEM Serial1 // serial to Quectel BC66

int delay_var = 0;

#include "secrets.h"
#include "TinyLoRa.h"
TinyLoRa lora = TinyLoRa(RFM_DIO0, RFM_CS, RFM_RST);

int msg_count = 0;
#define EN_SENSOR 4

//VL DEFS
#include "ComponentObject.h"
#include "RangeSensor.h"
#include "SparkFun_VL53L1X.h"
#include "vl53l1x_class.h"
#include "vl53l1_error_codes.h"

#define DIST_CHANGE 150 //mm ->15cm
#define MIN_TIME_LORA 30 //s
#define DISTANCE_THRESHOLD 800   //mm -> 80cm
const byte int_tof = 13; //PA20
const byte XSHUT = 6; //PA19
volatile bool int_flag = false;
SFEVL53L1X distanceSensor;

void send_lora(float dist){   

    lpp.addAnalogInput(0, dist/10.0);        
    lora.frameCounter = msg_count;
    lora.sendData(lpp.getBuffer(), lpp.getSize(), lora.frameCounter);
    Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
    msg_count++;
    lpp.reset();
}

void setup_BOD33(){
    Serial.println(F("Fuse settings before:"));
    Serial.println((*(uint32_t*)NVMCTRL_USER), HEX);         // Display the current user word 0 fuse settings
    Serial.println((*(uint32_t*)(NVMCTRL_USER + 4)), HEX);   // Display the current user word 1 fuse settings
    uint32_t userWord0 = *((uint32_t*)NVMCTRL_USER);            // Read fuses for user word 0
    uint32_t userWord1 = *((uint32_t*)(NVMCTRL_USER + 4));      // Read fuses for user word 1
    NVMCTRL->CTRLB.bit.CACHEDIS = 1;                            // Disable the cache
    NVMCTRL->ADDR.reg = NVMCTRL_AUX0_ADDRESS / 2;               // Set the address
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_EAR |                // Erase the auxiliary user page row
                         NVMCTRL_CTRLA_CMDEX_KEY;
    while(!NVMCTRL->INTFLAG.bit.READY)                          // Wait for the NVM command to complete
    NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;                 // Clear the error flags
    NVMCTRL->ADDR.reg = NVMCTRL_AUX0_ADDRESS / 2;               // Set the address
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC |                // Clear the page buffer
                         NVMCTRL_CTRLA_CMDEX_KEY;
    while(!NVMCTRL->INTFLAG.bit.READY)                          // Wait for the NVM command to complete
    NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;                 // Clear the error flags
    *((uint32_t*)NVMCTRL_USER) = userWord0 & ~FUSES_BOD33_EN_Msk;  // Disable the BOD33 enable fuse in user word 0
    *((uint32_t*)(NVMCTRL_USER + 4)) = userWord1;               // Copy back user word 1 unchanged
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_WAP  |               // Write to the user page
                         NVMCTRL_CTRLA_CMDEX_KEY;
    while(!NVMCTRL->INTFLAG.bit.READY)                          // Wait for the NVM command to complete
    NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;                 // Clear the error flags
    NVMCTRL->CTRLB.bit.CACHEDIS = 0;                            // Enable the cache
    Serial.println(F("Fuse settings after:"));
    Serial.println((*(uint32_t*)NVMCTRL_USER), HEX);         // Display the current user word 0 fuse settings
    Serial.println((*(uint32_t*)(NVMCTRL_USER + 4)), HEX);   // Display the current user word 1 fuse settings

}

//maybe not necessary
void nvm_wait_states(){
     NVMCTRL->CTRLB.bit.RWS = 3; 
}


void setup_rtc_debug(){
  byte h,mm,s = 0;  
  rtc.begin(); // initialize RTC  
  rtc.setTime(h, mm, s); 
  rtc.setAlarmTime(h, mm, s+15); 
  //rtc.enableAlarm(rtc.MATCH_SS); //rtc.MATCH_MMSS   RITAAAAA 
  //rtc.attachInterrupt(alarmMatch);
}


void i2c_scan(){
  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning...");

  for(address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of the Write.endTransmisstion 
    // to see if a device did acknowledge to the address.
    Serial.println("Scanning before begin ...");
    Wire.beginTransmission(address);
    Serial.println("Scanning after begin ...");
    
    error = Wire.endTransmission();
    Serial.println("Scanning after end trans ...");
    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error==4) {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}

//enable sensor - PA8

void initDistSensor(){

  
  
  sodaq_wdt_safe_delay(1000);
  //Serial.println("After Wire Begin!");
  
  float distance = 0;
  //Begin returns 0 on a good init    
  if (distanceSensor.begin() == 0) {
    //Serial.println("Sensor online!");
    sodaq_wdt_safe_delay(100);

    distanceSensor.setDistanceModeLong(); //set long distance mode, was short before
    
    distanceSensor.setTimingBudgetInMs(103); //test with 50, 140,200

    sodaq_wdt_safe_delay(100);
    
    distanceSensor.setInterruptPolarityLow();
 
    sodaq_wdt_safe_delay(100);
    
    distanceSensor.setIntermeasurementPeriod(8000);


    distanceSensor.setDistanceThreshold(DISTANCE_THRESHOLD , 1800, 0);

    Serial.println("THRESHOLDS: window,low,high");
    Serial.println(distanceSensor.getDistanceThresholdWindow()); 
    Serial.println(distanceSensor.getDistanceThresholdLow()); 
    Serial.println(distanceSensor.getDistanceThresholdHigh()); 

    
    distanceSensor.startRanging(); 
    distanceSensor.clearInterrupt();
    
    sodaq_wdt_safe_delay(500);
    Serial.println("sensor initialized");
  } else
    Serial.println("unable to init sensor");
}

void interrupt(){
  int_flag = true;
}

const byte DIO1 = A5;

void setup(){
    asm(".global _printf_float"); //very impotant for sprintf with float    
    Serial.begin(115200);
      // Bootup sodaq_wdt_safe_delay to programm the board.
    setup_BOD33();
    nvm_wait_states();
    
    //Find Reset Cause    
    if (REG_PM_RCAUSE == PM_RCAUSE_SYST){
      Serial.println("Reset requested by system");
      reset_cause = 1;
    }
    if (REG_PM_RCAUSE == PM_RCAUSE_WDT){
      Serial.println("Reset requested by Watchdog");
      reset_cause = 2;
    }
    if (REG_PM_RCAUSE == PM_RCAUSE_EXT){
      Serial.println("External reset requested");
      reset_cause = 3;
    }
    if (REG_PM_RCAUSE == PM_RCAUSE_BOD33){
      Serial.println("Reset brown out 3.3V");
      reset_cause = 4;
    }
    if (REG_PM_RCAUSE == PM_RCAUSE_BOD12){
      Serial.println("Reset brown out 1.2v");
      reset_cause = 5;
    }
    if (REG_PM_RCAUSE == PM_RCAUSE_POR){
      Serial.println("Normal power on reset");
      reset_cause = 0;
    }//End find reset cause

    Wire.begin();    
    sodaq_wdt_safe_delay(5000);    
    sodaq_wdt_enable(WDT_PERIOD_8X); //ENABLE WDT
    setup_rtc_debug();
  
    pinMode(MEDIR_CAP,OUTPUT);
    digitalWrite(MEDIR_CAP,LOW);
    pinMode(CAP_V,INPUT);
    analogReference(AR_DEFAULT);
    /*//LUX TINGS
    pinMode(LUX_ENABLE,OUTPUT);
    
    digitalWrite(LUX_ENABLE,LOW); 
    
    pinMode(LUX_ANALOG,INPUT); //965,966 in a lit room

    
    pinMode(EN_SENSOR,OUTPUT);
    digitalWrite(EN_SENSOR,HIGH);
    */
    pinMode(XSHUT,OUTPUT);
    digitalWrite(XSHUT,HIGH);

    pinMode(DIO1,INPUT);

    lora.setChannel(MULTI);
    lora.setDatarate(SF9BW125);
    if(!lora.begin()){
      Serial.println("Failed");
      Serial.println("Check your radio");
      while(true){
        delay(1000);
        Serial.println("Failed");
        Serial.println("Check your radio");
        delay(1000);
      }
    } else{
      Serial.println("INIT LORA SUCCESS");
    }

    
    initDistSensor();

    pinMode(int_tof, INPUT_PULLUP);
    //pinMode(int_tof, INPUT);

    attachInterrupt(int_tof, interrupt, FALLING);//Pin of interrupt is digital PIN2
    //stuff to allow falling interrupt
    // Set the XOSC32K to run in standby
    SYSCTRL->XOSC32K.bit.RUNSTDBY = 1;

    // Configure EIC to use GCLK1 which uses XOSC32K 
    // This has to be done after the first call to attachInterrupt()
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCM_EIC) | 
                        GCLK_CLKCTRL_GEN_GCLK1 | 
                        GCLK_CLKCTRL_CLKEN;
                        
    
    sodaq_wdt_reset();
    
    lora.sleep();
    
    distanceSensor.clearInterrupt();
  
    sodaq_wdt_safe_delay(1000);
    
    distanceSensor.clearInterrupt();

    digitalWrite(EN_SENSOR,LOW);
    
    if (sleep_flag){
        systemSleep();
    }
}

char buf;

/**
  Powers down all devices and puts the system to deep sleep.
*/
void systemSleep(){

    //digitalWrite(EN_SENSOR,LOW);
    
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    lora.sleep();
    
    // Disable USB
    //USBDevice.detach();
    
    Serial.flush();
    Serial.end();
    USBDevice.detach();
    USB->DEVICE.CTRLA.reg &= ~USB_CTRLA_ENABLE; // Disable USB
    //rtc.standbyMode();

    // Disable systick interrupt
    
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

    sodaq_wdt_disable();
    
    //BARRIER INSTRUCTION - WASNT HERE BEFORE
    __DSB();

    
    // SAMD sleep
    __WFI();

      
    // Enable systick interrupt
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

    sodaq_wdt_enable(WDT_PERIOD_8X); //ENABLE WDT

    //ENABLE 3.3V PWR_SUP
}


int cnt = 0;
int state = 0;
long last_send = 0;

void loop(){
      
  if (sodaq_wdt_flag) {
    // Reset watchdog
    sodaq_wdt_reset();
    sodaq_wdt_flag = false;
  
    if ((~int_flag)&&(~isButton)&&(~isClock)&&(~sodaq_wdt_flag)){
      if(sleep_flag){
          systemSleep();
      }
    }
  }
  if(int_flag){
    sodaq_wdt_reset();
    cnt++;
    
    if(cnt == 2){
      if(state){
        if(rtc.getEpoch() - last_send > MIN_TIME_LORA){
          int distance;

          digitalWrite(EN_SENSOR,HIGH);
          
          distance = distanceSensor.getDistance();//Get measurement distance.
          
          Serial.print("Distance(mm): ");
          Serial.println(distance);
          if(!lora.begin()){
            Serial.println("Failed");
            Serial.println("Check your radio");   
          } else{
            send_lora(distance);
            Serial.println("Send Data Lora");
            last_send = rtc.getEpoch();
  
            //UPDATE INT
            if(distance < DISTANCE_THRESHOLD){
              int min_value = distance - DIST_CHANGE;
              int max_value = distance + DIST_CHANGE;
              if(max_value > DISTANCE_THRESHOLD)
                max_value = DISTANCE_THRESHOLD;
              if(min_value < 20)
                min_value = 20; //lets make min distance 2cm
              distanceSensor.stopRanging();
              distanceSensor.clearInterrupt();
              distanceSensor.setDistanceThreshold(min_value, max_value, 2);
              distanceSensor.startRanging(); 
              distanceSensor.clearInterrupt();
              Serial.println("INT updated !");
            } else{
              distanceSensor.stopRanging();
              distanceSensor.clearInterrupt();
              // to change window of interrupt from 2 to 0 inicialization is needed
              initDistSensor();
              state = 0;
              Serial.println("INT updated && state = 0 !");
            }
            Serial.println("window,low,high: ");
            Serial.print(distanceSensor.getDistanceThresholdWindow()); 
            Serial.print(", ");Serial.print(distanceSensor.getDistanceThresholdLow()); 
            Serial.print(", ");Serial.println(distanceSensor.getDistanceThresholdHigh()); 
          }
        }  
      } else{
        if(rtc.getEpoch() - last_send > MIN_TIME_LORA){
          int distance;
          digitalWrite(EN_SENSOR,HIGH);
          distance = distanceSensor.getDistance();//Get measurement distance.
          
          Serial.print("Distance(mm): ");
          Serial.println(distance);
          if(!lora.begin()){
            Serial.println("Failed"); Serial.println("Check your radio");   
          }else{
            send_lora(distance);
            Serial.println("Send Data Lora");
            last_send = rtc.getEpoch();

            // UPDATE INT
            int min_value = distance - DIST_CHANGE;
            int max_value = distance + DIST_CHANGE;
            if(max_value > DISTANCE_THRESHOLD)
              max_value = DISTANCE_THRESHOLD;
            if(min_value < 20)
              min_value = 20; //lets make min distance 2cm
            distanceSensor.stopRanging();
            distanceSensor.clearInterrupt();
            distanceSensor.setDistanceThreshold(min_value, max_value, 2);
            distanceSensor.startRanging(); 
            distanceSensor.clearInterrupt();
            
            state = 1;
            Serial.println("INT updated && state = 1 !");
            Serial.println("window,low,high: ");
            Serial.print(distanceSensor.getDistanceThresholdWindow()); 
            Serial.print(", ");Serial.print(distanceSensor.getDistanceThresholdLow()); 
            Serial.print(", ");Serial.println(distanceSensor.getDistanceThresholdHigh()); 
          }
        }
      }
      cnt = 0;
    }

    int_flag = false;
    distanceSensor.clearInterrupt();

    digitalWrite(EN_SENSOR,LOW);
    
    if ((~int_flag)&&(~isButton)&&(~isClock)&&(~sodaq_wdt_flag)){
      if(sleep_flag){
        systemSleep();
      }
    }
  }
}
