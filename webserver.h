#pragma once

#include <Wifi.h>

class WebServer
{
  WiFiServer d_server;

  enum Event
  {
    VOLUME_UP_START,
    VOLUME_DOWN_START,
    STOP,
    EMPTY,

    EVENTLIST_SIZE = 10
  };

  Event d_eventList[EVENTLIST_SIZE];
  int d_currentEventIndex = 0;

public:
  WebServer(int port);
  void begin();
  bool handle();
  void execute();
  
}; // class WebServer