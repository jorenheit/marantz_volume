enum class State
{
  VOLUME_UP,
  VOLUME_DOWN,
  VOLUME_CHANGE_STOP,
  IDLE
};

enum class Command
{
  VOLUME_UP,
  VOLUME_DOWN,
  STOP
};

template <typename SignalType>
void RC5::Generator::execute()
{
  switch (d_currentState)
  {
    case State::IDLE               : return StateMachine<State::IDLE, SignalType>::execute(*this);
    case State::VOLUME_UP          : return StateMachine<State::VOLUME_UP, SignalType>::execute(*this);
    case State::VOLUME_DOWN        : return StateMachine<State::VOLUME_DOWN, SignalType>::execute(*this);
    case State::VOLUME_CHANGE_STOP : return StateMachine<State::VOLUME_CHANGE_STOP, SignalType>::execute(*this);
  }
}


template <State, Command>
struct StateMachine
{
  static void execute() {}; // nothing happens for unimplemented events
}


template <>
struct StateMachine<State::IDLE, Command::VOLUME_UP>
{
  static void execute()
  {
    if (d_generator.schedule<RC5::VolumeUp>())
    {
      d_activeClientIP = clientIP;
      d_currentState = State::VOLUME_UP;
    }
  }
};

template <>
struct StateMachine<State::IDLE, Command::VOLUME_DOWN>
{
  static void execute()
  {
    if (d_generator.schedule<RC5::VolumeDown>())
    {
      d_activeClientIP = clientIP;
      d_currentState = State::VOLUME_DOWN;
    }    
  }
};

template <>
struct StateMachine<State::IDLE, Command::STOP>
{
  static void execute()
  {
    // Stop signal arrived before action. Do nothing now but change
    // state to STOP. The next action signal from this client will
    // be executed once immediately.

    d_activeClientIP = clientIP;
    d_currentState = State::VOLUME_CHANGE_STOP;
  }
};

template <>
struct StateMachine<State::VOLUME_UP, Command::STOP>
{
  static void execute()
  {
    if (clientIP == d_activeClientIP) // only listen to stop command from the active client
    {
      d_generator.schedule<RC5::None>();
      d_currentState = State::IDLE;
    }    
  }
};

template <>
struct StateMachine<State::VOLUME_DOWN, Command::STOP>: 
  StateMachine<State::VOLUME_UP, Command::STOP>
{};

