
#include "network.h"
#include "webserver.h"
#include "menu.h"
#include "global_constants.h"

WebServer server{SERVER_PORT};
RC5::Generator generator{INTERRUPT_INTERVAL_MICROS};
Menu menu;

void setup() 
{
  // Power on LED active
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  pinMode(DATA_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("============== Device boot complete ==============");

  // Connect to network using information stored in EEPROM
  Network::connect();

  // Start web server
  server.begin();

  // Display the menu after boot
  menu.welcome();

  // Start the signal-generator
  generator.start();
}

void loop()
{
  menu.handle();

  if (Network::statusCheck())
    server.handle(generator);
  else
    Network::connect(false);
}