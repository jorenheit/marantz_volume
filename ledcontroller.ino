#include "ledcontroller.h"

Timer<TIMER_LED_BLINK> *LedController::s_timer = nullptr;

void LedController::init()
{
  pinMode(LED_PIN, OUTPUT);
  s_timer = new Timer<TIMER_LED_BLINK>(LED_BLINK_INTERVAL_MICROS, &LedController::blinkISR);
}

void LedController::on()
{
  s_timer->stop();
  digitalWrite(LED_PIN, HIGH);
}

void LedController::off()
{
  s_timer->stop();
  digitalWrite(LED_PIN, LOW);
}

void LedController::blink()
{
  s_timer->start();
}

void IRAM_ATTR LedController::blinkISR()
{
  static bool state = true;
  digitalWrite(LED_PIN, state);
  state = !state;
}