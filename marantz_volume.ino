
#include "network.h"
#include "webserver.h"
#include "menu.h"
#include "ledcontroller.h"
#include "global_constants.h"

LedController led;
WebServer server;
Menu menu;

void setup() 
{
  Serial.begin(115200);
  Serial.println("============== Device boot complete ==============");

  // blink until connection is established
  led.blink();

  // Connect to network using information stored in EEPROM
  Network::connect();

  // Start web server
  server.begin();

  // Display the menu after boot
  menu.welcome();
}

void loop()
{
  menu.handle();

  if (Network::statusCheck())
  {
    led.on();
    server.handle();
  }
  else
  {
    led.blink();
    Network::connect(false);
  }
}