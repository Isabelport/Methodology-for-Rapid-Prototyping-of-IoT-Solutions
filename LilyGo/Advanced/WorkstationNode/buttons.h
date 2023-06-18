
int pom = 0;  
int pom2 = 0;

//buttons pin
const int GREEN_BUTTON = 43; //pin 16
const int YELLOW_BUTTON = 44; //pin 21

struct button{
  int buttonState;
  int lastButtonState;
  unsigned long lastDebounceTime;  // the last time the output pin was toggled
  int newRequest;
  int aux_newRequest;
  int clicked;
};

button green = {HIGH, HIGH, 0, 0, 1, 0};
button yellow = {HIGH, HIGH, 0, 0, 1, 0};

unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


button read_button(button any, int pin){
  any.clicked = 0;
  int reading = digitalRead(pin);
  // check to see if you just pressed the button
  // since the last press to ignore any noise:
  if (reading != any.lastButtonState) {
    // reset the debouncing timer
    any.lastDebounceTime = millis();
  }

  if ((millis() - any.lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != any.buttonState) {
      any.buttonState = reading;

      // new button has request if the new button state is LOW (because of resistor pull up)
      if (any.buttonState == LOW) {
        any.newRequest = 1;
        any.aux_newRequest = 1; //only read request once
      }
      else
        any.newRequest = 0;

    }
  }
  
  if (any.newRequest && any.aux_newRequest){
    any.aux_newRequest = any.aux_newRequest - 1;
    any.clicked = 1;
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  any.lastButtonState = reading;

  return any;
}