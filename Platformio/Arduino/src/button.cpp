#include "button.h"

BUTTON::BUTTON() { }

int BUTTON::checkButton(int pin) {
  int event = 0;
  buttonVal = digitalRead(pin);
  // Button pressed down
  if (buttonVal == HIGH && buttonLast == LOW && (millis() - upTime) > debounce) {
    downTime = millis();
    ignoreUp = false;
    waitForUp = false;
    singleOK = true;
    holdEventPast = false;
    longHoldEventPast = false;
    if ((millis() - upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
    else  DConUp = false;
    DCwaiting = false;
    // Button released
  } else if (buttonVal == LOW && buttonLast == HIGH && (millis() - downTime) > debounce) {
    if (not ignoreUp) {
      upTime = millis();
      if (DConUp == false) DCwaiting = true;
      else{
        event = 2;
        DConUp = false;
        DCwaiting = false;
        singleOK = false;
      }
    }
  }
  if ( buttonVal == LOW && (millis() - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2) { // Test for normal click event: DCgap expired
    event = 1;
    DCwaiting = false;
  }
  // Test for hold
  if (buttonVal == HIGH && (millis() - downTime) >= holdTime) {
    if (not holdEventPast) { // Trigger "normal" hold
      event = 3;
      waitForUp = true;
      ignoreUp = true;
      DConUp = false;
      DCwaiting = false;
      //downTime = millis();
      holdEventPast = true;
    }
    if ((millis() - downTime) >= longHoldTime) { // Trigger "long" hold
      if (not longHoldEventPast) {
        event = 4;
        longHoldEventPast = true;
      }
    }
  }
  buttonLast = buttonVal;
  return event;
}
