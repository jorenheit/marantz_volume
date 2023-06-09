#pragma once

#include <WiFi.h>
#include "rc5.h"

class WebServer
{
  WiFiServer d_server;
  String const d_volumeUpUrl = "/volup";
  String const d_volumeDnUrl = "/voldn";
  static String const s_webpage;

public:
  WebServer(int port);
  void begin();
  bool handle(RC5::Generator &generator);
  
}; // class WebServer