/*
  link between the computer and the SoftSerial Shield
  at 9600 bps 8-N-1
  Computer is connected to Hardware UART
  SoftSerial Shield is connected to the Software UART:D2&D3
*/

#include <SoftwareSerial.h>
const int BUFFER_SIZE = 14;   // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const int DATA_TAG_SIZE = 8;  // 8byte tag

//#define implementation

#define rxPin 17              //amarelo
#define txPin 18              //branco
SoftwareSerial SoftSerial(rxPin, txPin);
unsigned char buffer[64];  // buffer array for data receive over serial port
int count = 0;             // counter for buffer array

void setup() {
  SoftSerial.begin(9600);  // the SoftSerial baud rate
  Serial.begin(9600);      // the Serial port of Arduino baud rate.
}

void loop() {
  // if date is coming from software serial port ==> data is coming from SoftSerial shield
  if (SoftSerial.available()) {
    while (SoftSerial.available())  // reading data into char array
    {
      buffer[count++] = SoftSerial.read();  // writing data into array
      if (count == 64) break;
    }

    Serial.write(buffer, count);  // if no data transmission ends, write buffer to hardware serial port
    clearBufferArray();           // call clearBufferArray function to clear the stored data from the array
    count = 0;                    // set counter of while loop to zero
  }
  if (Serial.available())             // if data is available on hardware serial port ==> data is coming from PC or notebook
    SoftSerial.write(Serial.read());  // write it to the SoftSerial shield
}
void clearBufferArray()  // function to clear buffer array
{
  // clear all index of array with command NULL
  for (int i = 0; i < count; i++) {
    buffer[i] = NULL;
  }
}
