
#include "network.h"
#include "webserver.h"
#include "webpage.h"

// Set web server port number to 80
WebServer server{80};

String welcome = "The following commands are available:\n"
"h - help\n"
"c - (re)connect to Wifi\n"
"s - enter SSID\n"
"p - enter password";

String prompt = "h/c/s/p: ";

void flushInputBuffer()
{
  while (Serial.available() > 0)
    Serial.read();
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("============== Device boot complete ==============");

  // Connect to network using information stored in EEPROM
  Network::connect();

  // Start web server
  server.begin();

  // Print welcome message to terminal
  Serial.println(welcome);
  Serial.println(prompt);
}

void loop()
{
  switch (Serial.read())
  {
    case 'h':
    {
      flushInputBuffer();
      Serial.println(welcome);
      Serial.println(prompt);
      break;
    }
    case 'c':
    {
      flushInputBuffer();
      Network::connect();
      Serial.println(prompt);
      break; 
    }
    case 's':
    {
      flushInputBuffer();
      Network::promptSSID();
      Serial.println(prompt);
      break; 
    }
    case 'p':
    {
      flushInputBuffer();
      Network::promptPassword();
      Serial.println(prompt);
      break; 
    }
  }

  server.handle();
  server.execute();
}