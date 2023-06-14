#pragma once
#include <esp32-hal-timer.h>

template <int TimerID>
class Timer
{
  static_assert(TimerID < 4, "Invalid Timer ID (must be < 4)");
  static bool used;

  hw_timer_t *s_hwtimer;

public:
  
  Timer(long micros, void (*isr)())
  {
    assert(!used && "TimerID used multiple times");

    static constexpr int PRESCALER = 80; // ticks at 1 MHz -> 1 us per tick
  
    s_hwtimer = timerBegin(TimerID, PRESCALER, true);
    timerAttachInterrupt(s_hwtimer, isr, true);
    timerAlarmWrite(s_hwtimer, micros, true);
    timerAlarmEnable(s_hwtimer);
  }

  void start()
  {
    timerStart(s_hwtimer);
  }

  void stop()
  {
    timerStop(s_hwtimer);
  }
  
};

template <int TimerID>
bool Timer<TimerID>::used = false;