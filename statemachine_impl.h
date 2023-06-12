template <Command>
struct CommandToSignalType;
#include "commandtosignaltype_impl.h"

template <State, Command>
struct StateMachine
{
  static void execute(WebServer &server, IPAddress const &) 
  {
    server.d_currentState = State::INVALID;
  }
};


// Implementations for State::IDLE:
//   Schedule the signal, change clientIP and move to next state

template <Command Cmd>
struct StateMachine<State::IDLE, Cmd>
{
  static void execute(WebServer &server, IPAddress const &clientIP)
  {
    using SignalType = typename CommandToSignalType<Cmd>::SignalType;

    server.d_generator.schedule<SignalType>();
    server.d_activeClientIP = clientIP;
    server.d_currentState = (Cmd == Command::STOP) ? State::INVALID : State::VOLUME_CHANGE; 
  }
};

// Implementations for State::VOLUME_CHANGE
//   Only STOP commands are valid

template <>
struct StateMachine<State::VOLUME_CHANGE, Command::STOP>
{
  static void execute(WebServer &server, IPAddress const &clientIP)
  {
    if (clientIP == server.d_activeClientIP)
    {
      using SignalType = typename CommandToSignalType<Command::STOP>::SignalType;

      server.d_generator.schedule<SignalType>();
      server.d_currentState = State::IDLE;
    }
  }
};

