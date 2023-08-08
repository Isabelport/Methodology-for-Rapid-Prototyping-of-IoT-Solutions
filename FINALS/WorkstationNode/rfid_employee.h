#include <SoftwareSerial.h>

const int BUFFER_SIZE = 14;   // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const int DATA_TAG_SIZE = 8;  // 8byte tag
#define rxPin 17
#define txPin 18
SoftwareSerial SoftSerial(rxPin, txPin);

uint8_t buffer[BUFFER_SIZE];  // used to store an incoming data frame
int buffer_index = 0;
int emp_id = -1;

const int num_of_employees = 14;
long int rfid_employee[num_of_employees] = { 7522210,   //1
                                             7278157,   //2
                                             5678679,   //3
                                             7363554,   //4
                                             11427961,  //5
                                             1,         //6
                                             2,         //7
                                             3,         //8
                                             4,         //9
                                             5,         //10
                                             6,         //11
                                             7,         //12
                                             8,         //13
                                             9 };       //14

String employee_name[num_of_employees + 1] = { "", "Antonio", "Daniela", "Diogo", "Elisabete", "Luis", "Joao", "Joaquim", "Lidia",
                                               "Maria", "Mario", "Patricia", "Raquel", "Ana", "Paulo" };  //0 is for unrecognized employee


//////////////REAL DATA
/*
String rfid_employee[num_of_employees] = { "0007263753",    //1
                                                 "0007263832",    //2
                                                 "0002157941",    //3
                                                 "0007263831",    //4
                                                 "0007263893",    //5
                                                 "0007263988",    //6
                                                 "0007263755",    //7
                                                 "0007263835",    //8
                                                 "0007263752",    //9
                                                 "0007263833",    //10
                                                 "0007263795",    //11
                                                 "0007263795",    //12
                                                 "0007232194",    //13
                                                 "0002152644" };  //14*/

//String employee_name[num_of_employees + 1] = { "", "Antonio", "Daniela", "Diogo", "Elisabete", "Isabel", "Joao", "Joaquim", "Lidia",
//"Maria", "Mario", "Patricia", "Raquel", "Ana", "Paulo" };  //0 is for unrecognized employee

void initRFIDSensor_employee() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  SoftSerial.begin(9600);
  SoftSerial.listen();

  Serial.println("RFID employee ready!");
}


long extractTag_employee() {

  uint8_t *msg_data_tag = buffer + 3;
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


bool compareRfid_employee(long rfid1, long rfid2) {
  int equal = 0;
  Serial.print("tag: ");
  Serial.println(rfid1);
  Serial.print("compare w: ");
  Serial.println(rfid2);
  if (rfid1 == rfid2) {
    Serial.println("yeye");
    return true;
  }

  return false;
}

int getCardId_employee(long tag) {
  int num_of_cards = num_of_employees;
  int isequal = 0;  //aux variable to compare rfid numbers
  int id = -1;
  for (int i = 0; i < num_of_cards; i++) {
    isequal = compareRfid_employee(tag, rfid_employee[i]);
    if (isequal) {
      id = i;
      break;
    }
  }
  id = id + 1;
  Serial.print("id: ");
  Serial.println(id);
  Serial.print(employee_name[id]);

  return id;
}


int readRFID_employee() {
  if (SoftSerial.available() > 0) {
    bool call_extract_tag = false;

    int ssvalue = SoftSerial.read();  // read
    if (ssvalue == -1) {              // no data was read
      return -1;
    }

    if (ssvalue == 2) {  // RDM630/RDM6300 found a tag => tag incoming
      buffer_index = 0;
    } else if (ssvalue == 3) {  // tag has been fully transmitted
      call_extract_tag = true;  // extract tag at the end of the function call
    }

    if (buffer_index >= BUFFER_SIZE) {  // checking for a buffer overflow (It's very unlikely that an buffer overflow comes up!)
      Serial.println("Error: Buffer overflow detected!");
      return -1;
    }

    buffer[buffer_index++] = ssvalue;  // everything is alright => copy current value to buffer

    if (call_extract_tag == true) {
      if (buffer_index == BUFFER_SIZE) {
        long tag = extractTag_employee();
        emp_id = getCardId_employee(tag);

      } else {  // something is wrong... start again looking for preamble (value: 2)
        buffer_index = 0;
        return -1;
      }
    }
  }
  return emp_id;
}
