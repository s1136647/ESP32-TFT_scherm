#include "Button.h"

Button::Button(int pin) : pin(pin), state(HIGH), lastState(HIGH), lastTime(0) {
  pinMode(pin, INPUT_PULLUP);
}

bool Button::pressed() {
  bool result = false;
  int reading = digitalRead(pin);

  if (reading != lastState) {
    lastTime = millis();
  }

  if (millis() - lastTime > DEBOUNCE_DELAY) {
    if (reading != state) {
      state = reading;
      if (state == LOW) {
        result = true;
      }
    }
  }

  lastState = reading;
  return result;
}
