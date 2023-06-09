#include "menu.h"

void Menu::flushInputBuffer()
{
  while (Serial.available() > 0)
    Serial.read();
}

void Menu::handle()
{
  switch (Serial.read())
  {
    case 'h':
    {
      executeOption([&](){
        Serial.println(d_welcomeMessage);
      });
      break;
    }
    case 'c':
    {
      executeOption([](){
        Network::connect();
      });
      break; 
    }
    case 's':
    {
      executeOption([](){
        Network::promptSSID();
      });
      break; 
    }
    case 'p':
    {
      executeOption([](){
        Network::promptPassword();
      });
      break; 
    }
  }
}

void Menu::welcome()
{
  Serial.println(d_welcomeMessage);
  Serial.println(d_prompt);
}