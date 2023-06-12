#pragma once

#include <WiFi.h>
#include "rc5.h"
#include "global_constants.h"


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

template <State, Command>
struct StateMachine;


class WebServer
{
  RC5::Generator d_generator{INTERRUPT_INTERVAL_MICROS};
  WiFiServer d_server;
  String const d_volumeUpUrl = "/volup";
  String const d_volumeDnUrl = "/voldn";
  String const d_stopUrl     = "/stop";
  static String const s_webpage;

  IPAddress d_activeClientIP;
  State d_currentState{State::IDLE};

public:
  WebServer(int port);
  void begin();
  bool handle();

private:
  template <State, Command>
  friend struct StateMachine;

  template <Command Cmd>
  void execute(IPAddress const &clientIP)
  {
    switch (d_currentState)
    {
      case State::IDLE          : StateMachine<State::IDLE          , Cmd>::execute(*this, clientIP); break;
      case State::VOLUME_CHANGE : StateMachine<State::VOLUME_CHANGE , Cmd>::execute(*this, clientIP); break;
      default                   : d_currentState = State::INVALID; break;
    }

    if (d_currentState == State::INVALID)
      reset();
  }

  void reset();
}; // class WebServer

#include "statemachine_impl.h"