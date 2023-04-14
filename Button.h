#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
public:
  Button(int pin);
  bool pressed();

private:
  const int pin;
  int state;
  int lastState;
  unsigned long lastTime;

  const unsigned long DEBOUNCE_DELAY = 200;
};

#endif
