#include "rc5.h"
#include "global_constants.h"

RC5::Generator *RC5::Generator::ISR::self = nullptr;

void IRAM_ATTR RC5::Generator::ISR::isr() 
{
  self->write(DATA_PIN);
}    

RC5::Generator::Generator(long const micros)
{
  ISR::self = this;
  d_timer = new Timer<0>(micros, &ISR::isr);
}

RC5::Generator::~Generator()
{
  delete d_timer;
}

void RC5::Generator::start()
{
  d_timer->start();
}

void RC5::Generator::stop()
{
  d_timer->stop();
  
}

void RC5::Generator::write(int const pin)
{
  static bool phase = 0;
  static Bit currentBit = IDLE_VALUE;

  if (phase == 0)
    currentBit = d_buffer.nextBit();

  digitalWrite(pin, (currentBit == IDLE_VALUE) ? 0 : !(currentBit ^ phase));
  phase = !phase;

  /*  
        0   1    (phase)
      ___ 
  0 =     |___   (H -> L)
  
          ___
  1 =  ___|      (L -> H)

  phase bit output
  0     0   1
  0     1   0
  1     0   0
  1     1   1

  ==> output = !(bit ^ phase)
  */
}
