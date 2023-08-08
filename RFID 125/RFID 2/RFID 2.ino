// (c) Michael Schoeffler 2018, http://www.mschoeffler.de
#include <SoftwareSerial.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

const int BUFFER_SIZE = 14;   // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const int DATA_TAG_SIZE = 8;  // 8byte tag

//#define implementation

#define rxPin 17
#define txPin 18
SoftwareSerial SoftSerial(rxPin, txPin);
//SoftwareSerial SoftSerial(17, 18);


uint8_t buffer[BUFFER_SIZE];  // used to store an incoming data frame
int buffer_index = 0;
int emp_id = -1;

const int num_of_employees = 14;
long int rfid_employee[num_of_employees] = { 7522210,  //1
                                             7278157,  //2
                                             5678679,  //3
                                             7363554,  //4
                                             0,        //5
                                             1,        //6
                                             2,        //7
                                             3,        //8
                                             4,        //9
                                             5,        //10
                                             6,        //11
                                             7,        //12
                                             8,        //13
                                             9 };      //14

String employee_name[num_of_employees + 1] = { "", "Antonio", "Daniela", "Diogo", "Elisabete", "Isabel", "Joao", "Joaquim", "Lidia",
                                               "Maria", "Mario", "Patricia", "Raquel", "Ana", "Paulo" };  //0 is for unrecognized employee

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  //TFT
  tft.init();
  tft.setRotation(3);  //1 horizontal cabo do lado direito // 3 horizontal cabo do lado esquerdo
  tft.fillScreen(TFT_PINK);

  tft.fillRectVGradient(0, 60, 160, 50, TFT_ORANGE, TFT_RED);
  tft.setCursor(10, 70);
  tft.print("Vertical gradient");
  delay(1000);

#ifndef implementation
  Serial.begin(115200);
#endif

  SoftSerial.begin(9600);
  SoftSerial.listen();
  Serial.println("INIT DONE");
}

void loop() {

  if (SoftSerial.available() > 0) {
    bool call_extract_tag = false;

    int ssvalue = SoftSerial.read();  // read
    if (ssvalue == -1) {              // no data was read
      return;
    }

    if (ssvalue == 2) {  // RDM630/RDM6300 found a tag => tag incoming
      buffer_index = 0;
    } else if (ssvalue == 3) {  // tag has been fully transmitted
      call_extract_tag = true;  // extract tag at the end of the function call
    }

    if (buffer_index >= BUFFER_SIZE) {  // checking for a buffer overflow (It's very unlikely that an buffer overflow comes up!)
      Serial.println("Error: Buffer overflow detected!");
      return;
    }

    buffer[buffer_index++] = ssvalue;  // everything is alright => copy current value to buffer

    if (call_extract_tag == true) {
      if (buffer_index == BUFFER_SIZE) {
        long tag = extractTag();
        emp_id = getCardId(tag);

      } else {  // something is wrong... start again looking for preamble (value: 2)
        buffer_index = 0;
        return;
      }
    }
  }
}

long extractTag() {

  //  uint8_t msg_head = buffer[0];
  //uint8_t *msg_data = buffer + 1; // 10 byte => data contains 2byte version + 8byte tag
  //uint8_t *msg_data_version = msg_data;
  uint8_t *msg_data_tag = buffer + 3;

  // print message that was sent from RDM630/RDM6300
  char msg_tag_str[DATA_TAG_SIZE] = "";
  char *pointer_tag = msg_tag_str;
  for (int i = 0; i < DATA_TAG_SIZE; ++i) {
    msg_tag_str[i] = char(msg_data_tag[i]);
  }

  long msg_tag = 0;
  msg_tag = strtol(msg_tag_str, NULL, 16);

  Serial.print("msg_tag ");
  Serial.println(msg_tag);

  return msg_tag;
}

int getCardId(long tag) {
  int num_of_cards = num_of_employees;
  int isequal = 0;  //aux variable to compare rfid numbers
  int id = -1;
  for (int i = 0; i < num_of_cards; i++) {
    isequal = compareRfid(tag, rfid_employee[i]);
    if (isequal) {
      id = i;
      break;
    }
  }
  id = id + 1;
  Serial.print("id: ");
  Serial.println(id);
  Serial.println(employee_name[id]);

  return id;
}


bool compareRfid(long rfid1, long rfid2) {
  int equal = 0;
  Serial.print("compare w: ");
  Serial.println(rfid2);
  if (rfid1 == rfid2) {
    Serial.println("yeye");
    return true;
  }

  return false;
}