#include "ledcontroller.h"

LedController::LedController():
  d_timer(LED_BLINK_INTERVAL_MICROS, &LedController::blinkISR)
{
  pinMode(LED_PIN, OUTPUT);
}

void LedController::on()
{
  d_timer.stop();
  digitalWrite(LED_PIN, HIGH);
}

void LedController::off()
{
  d_timer.stop();
  digitalWrite(LED_PIN, LOW);
}

void LedController::blink()
{
  d_timer.start();
}

void IRAM_ATTR LedController::blinkISR()
{
  static bool state = true;
  digitalWrite(LED_PIN, state);
  state = !state;
}