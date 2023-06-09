#pragma once

#include "network.h"

class Menu
{
  String const d_welcomeMessage = "The following commands are available:\n"
  "h - help\n"
  "c - (re)connect to Wifi\n"
  "s - enter SSID\n"
  "p - enter password";

  String const d_prompt = "h/c/s/p: ";

  void flushInputBuffer();

  template <typename F>
  void executeOption(F&& fun);

public:

  void handle();
  void welcome();

}; // class Menu

template <typename F>
void Menu::executeOption(F&& fun)
{
  flushInputBuffer();
  fun();
  Serial.println(d_prompt);
}
