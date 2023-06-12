#include "rc5.h"
#include "global_constants.h"

RC5::Generator *RC5::Generator::ISR::self = nullptr;

void IRAM_ATTR RC5::Generator::ISR::isr() 
{
  self->write();
}    

RC5::Generator::Generator(long const micros)
{
  ISR::self = this;
  pinMode(DATA_PIN, OUTPUT);
  d_timer = new Timer<TIMER_SIGNAL>(micros, &ISR::isr);
}

RC5::Generator::Buffer::Buffer()
{
  reset();
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

RC5::Bit IRAM_ATTR RC5::Generator::Buffer::nextBit()
{ 
  // Get next bit and increment index (wrap around)
  Bit bit = d_buffer[d_currentBuffer][d_currentIndex++];
  d_currentIndex %= BUFSIZE;

  // Check if we need to switch buffers in case the index just 
  // wrapped around.
  if (d_currentIndex == 0)
  {
    if (d_newSignalScheduled)
    {
      // Switch buffers
      d_currentBuffer = !d_currentBuffer;
      d_newSignalScheduled = false;
    }
  }

  return bit;
}

void IRAM_ATTR RC5::Generator::write()
{
  static bool phase = 0;
  static Bit currentBit = IDLE_VALUE;

  if (phase == 0)
    currentBit = d_buffer.nextBit();

  digitalWrite(DATA_PIN, (currentBit == IDLE_VALUE) ? 0 : !(currentBit ^ phase));
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

void RC5::Generator::reset()
{
  stop();
  {
    digitalWrite(DATA_PIN, 0);
    d_buffer.reset();
  }
  start();
}

void RC5::Generator::Buffer::reset()
{
  for (int i = 0; i != BUFSIZE; ++i)
  {
    d_buffer[0][i] = IDLE_VALUE;
    d_buffer[1][i] = IDLE_VALUE;
  }

  d_currentIndex = 0;
  d_currentBuffer = 0;
  d_newSignalScheduled = false;
  d_toggleBit = 0;
}
