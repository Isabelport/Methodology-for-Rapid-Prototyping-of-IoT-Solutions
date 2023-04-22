
#define SERIAL_DEBUG_BAUD    115200
#define YES 1
#define NO 0

const int sensor =  4;    
int state = -1;
int prev_state = -1;
int sendUpdate = -1;

int newState(int state){

  if (state == prev_state){
    return NO;
  }
  else{
    prev_state = state;
    return YES;
  }
}

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  pinMode(sensor, INPUT);

}

void loop() {
  state = digitalRead(sensor);
  Serial.println(state);
  //sendUpdate = 1 if new state 
  sendUpdate = newState(state);
  //Send to TB new information
  if(sendUpdate){
    Serial.print("Send data: ");
    Serial.println(state);
  }

  delay(100);
}