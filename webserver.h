#pragma once

#include <WiFi.h>
#include "rc5.h"

enum class State
{
  IDLE,
  VOLUME_CHANGE,
  INVALID
};

enum class Command
{
  VOLUME_UP,
  VOLUME_DOWN,
  STOP
};

template <Command>
struct CommandToSignalType;
#include "commandtosignaltype_impl.h"

class WebServer
{
  RC5::Generator d_generator;
  WiFiServer d_server;
  String const d_volumeUpUrl = "/volup";
  String const d_volumeDnUrl = "/voldn";
  String const d_stopUrl     = "/stop";
  static String const s_webpage;

  IPAddress d_activeClientIP;
  State d_currentState{State::IDLE};

public:
  WebServer();
  void begin();
  bool handle();

private:
  void reset();

  template <Command Cmd>
  void execute(IPAddress const &clientIP);
}; // class WebServer


// Statemachine to handle commands:
template <Command Cmd>
void WebServer::execute(IPAddress const &clientIP)
{
  switch (d_currentState)
  {
    case State::IDLE:
    {
      d_generator.schedule<typename CommandToSignalType<Cmd>::SignalType>();
      d_activeClientIP = clientIP;
      d_currentState = (Cmd == Command::STOP) ? State::INVALID : State::VOLUME_CHANGE; 
      break;
    }
    case State::VOLUME_CHANGE:
    {
      if constexpr (Cmd == Command::STOP)
      {
        if (clientIP == d_activeClientIP)
        {
          d_generator.schedule<RC5::None>();
          d_currentState = State::IDLE;
        }
      }
      else
      {
        d_currentState = State::INVALID;
      }
      break;
    }
  }

  if (d_currentState == State::INVALID)
    reset();
}