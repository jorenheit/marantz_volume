
#include "network.h"
#include "webserver.h"
#include "menu.h"
#include "ledcontroller.h"
#include "global_constants.h"

WebServer server{SERVER_PORT};
Menu menu;

void setup() 
{
  Serial.begin(115200);
  Serial.println("============== Device boot complete ==============");

  LedController::init();
  LedController::on();

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
    LedController::on();
    server.handle();
  }
  else
  {
    LedController::blink();
    Network::connect(false);
  }
}