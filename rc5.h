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
    VOL_UP = 0x10,
    VOL_DN = 0x11,
    MUTE   = 0x0D,
    POWER  = 0x0C,
    NO_OP  = 0x00
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

  using VolUp = Signal<AMP, VOL_UP>;
  using VolDn = Signal<AMP, VOL_DN>;
  using Mute =  Signal<AMP, MUTE>;
  using Power =  Signal<AMP, POWER>;

  template <typename SignalType>
  constexpr Bit Package[N_BITS] =   {
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


  class Generator
  {
    template <int BufferSize>
    class Buffer
    {
      Bit d_buffer[BufferSize * N_BITS];
      int d_pushIndex = 0;
      int d_nextIndex = 0;

    public:
      Buffer();

      template<typename SignalType>
      void push(bool const toggle = true);
      
      Bit nextBit();
    };

    Buffer<10> d_buffer; // buffers 10 commands
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
    void push(bool const toggle = true);

  }; // class Generator

// TEMPLATE IMPLEMENTATIONS

  template <typename SignalType>
  void Generator::push(bool const toggle)
  {
    // While pushing signals, stop the timer to make
    // sure the entire package is in the buffer when the 
    // timer fires

    d_timer->stop();
    {
      d_buffer.push<SignalType>(toggle);
    }
    d_timer->start();

    //delay(50); // small delay to separate commands
  }

  template <int BufferSize>
  Generator::Buffer<BufferSize>::Buffer()
  {
    memset(&d_buffer[0], IDLE_VALUE, sizeof(d_buffer));
  }

  template <int BufferSize>
  template<typename SignalType>
  void Generator::Buffer<BufferSize>::push(bool const toggle)
  {
    static bool toggleValue = 1;

    memcpy(&d_buffer[d_pushIndex * N_BITS], &Package<SignalType>[0], N_BITS);
    if (toggle)
    {
      d_buffer[d_pushIndex * N_BITS + TOGGLE_BIT] = toggleValue;
      toggleValue = !toggleValue;
    }

    if (++d_pushIndex == BufferSize)
      d_pushIndex = 0;
  }

  template <int BufferSize>
  Bit Generator::Buffer<BufferSize>::nextBit()
  {
    Bit &bit = d_buffer[d_nextIndex];
    Bit const copy = bit;
    if (bit != IDLE_VALUE)
    {
      ++d_nextIndex;
      d_nextIndex %= BufferSize * N_BITS;
      bit = IDLE_VALUE;
    }

    return copy;
  }

} // namespace RC5



