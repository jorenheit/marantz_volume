#pragma once
#include "global_constants.h"

class LedController
{
  static Timer<TIMER_LED_BLINK> *s_timer;

public:
  static void init();
  static void on();
  static void off();
  static void blink();

private:
  static void blinkISR();
};