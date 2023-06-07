#pragma once

#include <WiFi.h>
#include <EEPROM.h>

class Network
{
  struct Memory
  {
    // Network settings (SSID/Password) are stored in EEPROM memory.
    // The memory is organised as follows:
    //
    // 0          1               1 + SSID Bufsize
    // GOOD-flag  Start SSID buf  Start of PW buf
    enum
    {
      GOOD_FLAG_ADDRESS = 0,
      GOOD_FLAG_VALUE = 0xAB,

      SSID_BUFFER_SIZE = 50,
      PASSWORD_BUFFER_SIZE = 50,

      SSID_BUFFER_START = 1,
      PASSWORD_BUFFER_START = 1 + (1 + SSID_BUFFER_SIZE),

      SIZE = 1 + (1 + SSID_BUFFER_SIZE) + (1 + PASSWORD_BUFFER_SIZE)
    };
  };

  static bool s_initialized;
  static String s_ssid;
  static String s_password;

public:
  static bool init();
  static void connect();
  
  static String const &getSSID();
  static String const &getPassword();
  static String promptSSID();
  static String promptPassword();


private:
  static bool reset();
  static void fetchSSID();
  static void fetchPassword();
  static bool setSSID(String const ssid);
  static bool setPassword(String const password);

}; // class Network