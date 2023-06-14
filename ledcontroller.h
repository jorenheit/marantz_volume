#pragma once
#include "global_constants.h"

class LedController
{
  Timer<TIMER_LED_BLINK> d_timer;

public:
  LedController();
  void on();
  void off();
  void blink();

private:
  static void blinkISR();
};