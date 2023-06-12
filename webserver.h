#pragma once

#include <WiFi.h>
#include "rc5.h"
#include "global_constants.h"

class WebServer
{
  RC5::Generator d_generator{INTERRUPT_INTERVAL_MICROS};
  WiFiServer d_server;
  String const d_volumeUpUrl = "/volup";
  String const d_volumeDnUrl = "/voldn";
  String const d_stopUrl     = "/stop";
  static String const s_webpage;

  IPAddress d_activeClientIP;

  enum class State
  {
    VOLUME_UP,
    VOLUME_DOWN,
    VOLUME_CHANGE_STOP,
    IDLE
  };

  State d_currentState{State::IDLE};

public:
  WebServer(int port);
  void begin();
  bool handle();

private:
   void volumeUp(IPAddress const &clientIP);
   void volumeDown(IPAddress const &clientIP);
   void volumeStop(IPAddress const &clientIP);
  
}; // class WebServer