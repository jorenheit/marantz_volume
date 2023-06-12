template <>
struct CommandToSignalType<Command::VOLUME_UP>
{
  using SignalType = RC5::VolumeUp;
};

template <>
struct CommandToSignalType<Command::VOLUME_DOWN>
{
  using SignalType = RC5::VolumeDown;
};

template <>
struct CommandToSignalType<Command::STOP>
{
  using SignalType = RC5::None;
};