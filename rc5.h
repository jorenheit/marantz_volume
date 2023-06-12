#pragma once
#include "timer.h"

namespace RC5
{
  using Bit = byte;

  enum Address
  {
    AMP = 0x10,
    CD  = 0x14,
    DVD = 0x19,
    NO_ADDR = 0x00,
  };

  enum Command
  {
    VOLUME_UP = 0x10,
    VOLUME_DN = 0x11,
    NO_OP     = 0x00
  };

  static constexpr byte TOGGLE_BIT = 2;
  static constexpr byte N_BITS = 14;
  static constexpr Bit IDLE_VALUE = -1;

  template <byte Address, byte Command>
  struct Signal
  {
    static constexpr byte address = Address;
    static constexpr byte command = Command;
  };

  using VolumeUp   = Signal<AMP, VOLUME_UP>;
  using VolumeDown = Signal<AMP, VOLUME_DN>;
  using None       = Signal<NO_ADDR, NO_OP>;

  template <typename SignalType>
  constexpr Bit package[N_BITS] =   {
    1, 
    1,
    0, // togglebit 

    !!(SignalType::address & (1 << 4)),
    !!(SignalType::address & (1 << 3)),
    !!(SignalType::address & (1 << 2)),
    !!(SignalType::address & (1 << 1)),
    !!(SignalType::address & (1 << 0)),

    !!(SignalType::command & (1 << 5)),
    !!(SignalType::command & (1 << 4)),
    !!(SignalType::command & (1 << 3)),
    !!(SignalType::command & (1 << 2)),
    !!(SignalType::command & (1 << 1)),
    !!(SignalType::command & (1 << 0))
  };

  template<>
  constexpr Bit package<None>[N_BITS] = {
    IDLE_VALUE,
    IDLE_VALUE,
    IDLE_VALUE,

    IDLE_VALUE,
    IDLE_VALUE,
    IDLE_VALUE,
    IDLE_VALUE,
    IDLE_VALUE,

    IDLE_VALUE,
    IDLE_VALUE,
    IDLE_VALUE,
    IDLE_VALUE,
    IDLE_VALUE,
    IDLE_VALUE
  };

  class Generator
  {
    class Buffer
    {
      // Double buffer: new events will be scheduled in the second buffer.
      // We will switch buffers whenever a new signal is present and the 
      // current signal has been emitted.

      // The first part of a buffer contains an RC5 signal, while the second
      // half contains a bunch of IDLE-bits to separate consecutive commands
      // by around 25 ms. The IDLE bits are set in the constructor and never
      // accessed afterwards.

      static constexpr int BUFSIZE = 4 * N_BITS;
      Bit d_buffer[2][BUFSIZE];
      
      //volatile bool d_repeat[2]  = {true, true};
      volatile int d_currentIndex = 0;
      volatile bool d_currentBuffer = 1;
      volatile bool d_newSignalScheduled = false;
      volatile bool d_toggleBit = 0;

    public:
      Buffer();
      Bit nextBit();

      template <typename SignalType>
      bool schedule(/*bool const repeat*/);

    }; // class Buffer

    Buffer d_buffer;
    Timer<0> *d_timer = nullptr;

    struct ISR
    {
      static Generator *self;
      static void isr();
    };

  public:
    Generator(long const micros);
    ~Generator();

    void start();
    void stop();
    void write(int const pin);

    template <typename SignalType>
    void schedule(/*bool const repeat = true*/);

  }; // class Generator

// TEMPLATE IMPLEMENTATIONS

  template <typename SignalType>
  void Generator::schedule(/*bool const repeat*/)
  {
    bool success = false;
    while (!success)
    {
      stop();
      {
        success = d_buffer.schedule<SignalType>(/*repeat*/);
      }
      start();
    }
  }

  template <typename SignalType>
  bool Generator::Buffer::schedule(/*bool const repeat*/)
  {
    if (d_newSignalScheduled)
      return false;

    bool nextBuffer = !d_currentBuffer;

    memcpy(&d_buffer[nextBuffer][0], &package<SignalType>[0], N_BITS);
    //memcpy(&d_buffer[nextBuffer][2 * N_BITS], &package<SignalType>[0], N_BITS);
    //d_repeat[nextBuffer] = repeat;

    // Is this even necessary???!!!
    if constexpr (SignalType::command != Command::NO_OP)
    {
      d_buffer[nextBuffer][TOGGLE_BIT] = d_toggleBit;
      //d_buffer[nextBuffer][2 * N_BITS + TOGGLE_BIT] = d_toggleBit;
      d_toggleBit = !d_toggleBit;
    }

    d_newSignalScheduled = true;
    return true; 
  }  

} // namespace RC5



