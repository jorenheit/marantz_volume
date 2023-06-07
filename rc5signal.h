#pragma once

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

  template <word BufferSize>
  class Buffer
  {
    Bit buffer[BufferSize * N_BITS];
    int pushIndex = 0;
    int nextIndex = 0;

  public:
    Buffer()
    {
      memset(&buffer[0], IDLE_VALUE, sizeof(buffer));
    }

    template<typename SignalType>
    void pushSignal(bool const toggle = true)
    {
      static bool toggleValue = 1;

      cli();
      memcpy(&buffer[pushIndex * N_BITS], &Package<SignalType>[0], N_BITS);
      if (toggle)
      {
        buffer[pushIndex * N_BITS + TOGGLE_BIT] = toggleValue;
        toggleValue = !toggleValue;
      }
      sei();

      if (++pushIndex == BufferSize)
        pushIndex = 0;
    }

    Bit nextBit()
    {
      Bit &bit = buffer[nextIndex];
      Bit const copy = bit;
      if (bit != IDLE_VALUE)
      {
        if (++nextIndex == BufferSize * N_BITS)
          nextIndex = 0;
        bit = IDLE_VALUE;
      }

      return copy;
    }
  };
  
  class Generator
  {
    Buffer<20> buffer; // buffers 20 commands

  public:

    void write(int const pin)
    {
      static bool phase = 0;
      static Bit currentBit = IDLE_VALUE;

      if (phase == 0)
        currentBit = buffer.nextBit();

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

    template <typename SignalType>
    void send(bool const toggle = true)
    {
      buffer.pushSignal<SignalType>(toggle);
      delay(50); // small delay to separate commands
    }
  }; // class Generator

} // namespace RC5

constexpr auto DEVICE = RC5::AMP; 
using VolUp = RC5::Signal<DEVICE, RC5::VOL_UP>;
using VolDn = RC5::Signal<DEVICE, RC5::VOL_DN>;
using Mute =  RC5::Signal<DEVICE, RC5::MUTE>;
using Power =  RC5::Signal<DEVICE, RC5::POWER>;

