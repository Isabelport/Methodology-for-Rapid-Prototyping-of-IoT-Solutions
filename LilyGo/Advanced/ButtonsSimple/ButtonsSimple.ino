// constants won't change. They're used here to set pin numbers:
const int buttonPin = 16;  // the number of the pushbutton pin
//const int ledPin = 13;    // the number of the LED pin

// Variables will change:
//int ledState = HIGH;        // the current state of the output pin
int buttonState = HIGH;            // the current reading from the input pin
int lastButtonState = HIGH;  // the previous reading from the input pin
int aux_new_task = 1; // just sends request once
int new_task = 0;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(buttonPin, INPUT);
  //pinMode(ledPin, OUTPUT);

  // set initial LED state
  //digitalWrite(ledPin, ledState);
}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
        new_task = 1;
        aux_new_task = 1;
        //ledState = !ledState;
      }
      else 
        new_task = 0;
    }
  }
  if (new_task && aux_new_task){
    Serial.println("NEW TASK");
    aux_new_task = aux_new_task - 1;
  }
  // set the LED:
  //digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
