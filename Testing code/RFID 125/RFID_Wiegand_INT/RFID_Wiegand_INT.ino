//  RFID_Wiegand_INT.pde  to read card ID number from the RFID with
// wiegand interface.
//  see http://www.seeedstudio.com/depot/ for details on 125K RFID module

//  Copyright (c) 2010 seeed technology inc.
//  Author: Icing Chang
//  Version: september 2, 2010
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

byte RFIDcardNum[4];
byte evenBit = 0;
byte oddBit = 0;
byte isData0Low = 0;
byte isData1Low = 0;
int recvBitCount = 0;
byte isCardReadOver = 0;

void setup()
{
  Serial.begin(9600);
  attachInterrupt(0, ISRreceiveData0, FALLING );  //data0/tx is connected to pin 2, which results in INT 0
  attachInterrupt(1, ISRreceiveData1, FALLING );  //data1/rx is connected to pin 3, which results in INT 1

}

void loop()
{
  //read card number bit
  if(isData0Low||isData1Low){
    if(1 == recvBitCount){//even bit
      evenBit = (1-isData0Low)&isData1Low;
    }
    else if( recvBitCount >= 26){//odd bit
      oddBit = (1-isData0Low)&isData1Low;
      isCardReadOver = 1;
      delay(10);
    }
    else{
      //only if isData1Low = 1, card bit could be 1
      RFIDcardNum[2-(recvBitCount-2)/8] |= (isData1Low << (7-(recvBitCount-2)%8));
    }
    //reset data0 and data1
    isData0Low = 0;
    isData1Low = 0;
  }

  //print the card id number
  if(isCardReadOver){
    if(checkParity()){
    //Serial.println();
    //Serial.print(RFIDcardNum[2],HEX);//High byte
    //Serial.print(RFIDcardNum[1],HEX);//Midle byte
    //Serial.println(RFIDcardNum[0],HEX);//Low byte
      Serial.println(*((long *)RFIDcardNum));
    }
    resetData();
  }

}

byte checkParity(){
  int i = 0;
  int evenCount = 0;
  int oddCount = 0;
  for(i = 0; i < 8; i++){
    if(RFIDcardNum[2]&(0x80>>i)){
      evenCount++;
    }
  }
  for(i = 0; i < 4; i++){
    if(RFIDcardNum[1]&(0x80>>i)){
      evenCount++;
    }
  }
  for(i = 4; i < 8; i++){
    if(RFIDcardNum[1]&(0x80>>i)){
      oddCount++;
    }
  }
  for(i = 0; i < 8; i++){
    if(RFIDcardNum[0]&(0x80>>i)){
      oddCount++;
    }
  }
  
  if(evenCount%2 == evenBit && oddCount%2 != oddBit){
    return 1;
  }
  else{
    return 0;
  }
}
void resetData(){
  RFIDcardNum[0] = 0;
  RFIDcardNum[1] = 0;
  RFIDcardNum[2] = 0;
  RFIDcardNum[3] = 0;
  evenBit = 0;
  oddBit = 0;
  recvBitCount = 0;
  isData0Low = 0;
  isData1Low = 0;
  isCardReadOver = 0;
}
// handle interrupt0
void ISRreceiveData0(){
  recvBitCount++;
  isData0Low = 1;
}

// handle interrupt1
void ISRreceiveData1(){
  recvBitCount++;
  isData1Low = 1;
}

