#pragma once
#include "timer.h"
#include "global_constants.h"

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
      PASSWORD_BUFFER_START = SSID_BUFFER_START + (1 + SSID_BUFFER_SIZE),

      SIZE = 1 + (1 + SSID_BUFFER_SIZE) + (1 + PASSWORD_BUFFER_SIZE),
      MAX_EEPROM_BYTES = 512
    };

    static_assert(SIZE <= MAX_EEPROM_BYTES, "too many bytes allocated to EEPROM");
  };

  static bool s_initialized;
  static String s_ssid;
  static String s_password;
  volatile static bool s_statusFlag;

  static Timer<TIMER_NETWORK_CHECK> *s_timer;

public:
  static void connect(bool configure = true);
  static bool statusCheck();
  
  static void promptSSID();
  static void promptPassword();


private:
  static void setStatusFlag();
  static bool init();
  static bool init(String const &hostname);
  static bool reset();
  
  static void fetchSSID();
  static void fetchPassword();

  static bool setSSID(String const &ssid);
  static bool setPassword(String const &password);
}; // class Network