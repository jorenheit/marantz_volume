#include <WiFi.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include "network.h"

bool Network::s_initialized = false;
String Network::s_ssid = "";
String Network::s_password = "";
volatile bool Network::s_statusFlag = false;
Timer<1> *Network::s_timer = nullptr;

bool Network::init()
{
  if (s_initialized)
    return true;

  if (!EEPROM.begin(Memory::SIZE))
    return false;

  byte const good = EEPROM.read(Memory::GOOD_FLAG_ADDRESS);
  if (good != Memory::GOOD_FLAG_VALUE)
    if (reset() == false)
      return false;

  s_initialized = true;
  fetchSSID();
  fetchPassword();

  // Setup timer to check network status 
  s_timer = new Timer<TIMER_NETWORK_CHECK>(WIFI_CHECK_INTERVAL, &setStatusFlag);
  s_timer->start();

  return true;
}

void Network::connect(bool configure)
{
  if (!s_initialized)
    init();
  
  if (configure)
  {
    if (s_ssid == "")
    {
      Serial.println("Wifi not configured.");
      promptSSID();
      promptPassword();
    }
    else if (s_password == "")
    {
      Serial.println("No password set for " + s_ssid + ".");
      promptPassword();
    }
  }

  Serial.print("Connecting to ");
  Serial.print(s_ssid);

  WiFi.disconnect(); // break any previous connections
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(s_ssid, s_password);

  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count <  WIFI_CONNECTION_TIMEOUT) 
  {
    delay(1000);
    Serial.print(".");
    ++count;
  }

  if (count != WIFI_CONNECTION_TIMEOUT) // CONNECTED!    
  {
    Serial.print("\nConnected\nIP address: ");
    Serial.println(WiFi.localIP());

    bool mdnsStatus = false;
    for (int i = 0; i != MDNS_MAX_TRIES; ++i)
    {
      if (MDNS.begin(HOSTNAME))
      {
        mdnsStatus = true;
        break;
      }
      delay(1000);
    }

    Serial.println(mdnsStatus ? "MDNS OK" : "MDNS failed");

    s_statusFlag = true;
    return;
  }

  Serial.println("\nCould not connect to network.");
}

void IRAM_ATTR Network::setStatusFlag()
{
  s_statusFlag = (WiFi.status() == WL_CONNECTED);
}

bool Network::statusCheck()
{
  return s_statusFlag;
}

bool Network::setSSID(String const &ssid)
{
  if (!s_initialized) 
  {
    Serial.println("Could not set ssid. Did you call init()?");
    return false;
  }

  int const len = ssid.length();
  if (len > Memory::SSID_BUFFER_SIZE)
  {
    Serial.println("ssid length exceeds maximum (" + String(Memory::SSID_BUFFER_SIZE) + ")");
    return false;
  }

  EEPROM.write(Memory::SSID_BUFFER_START, len);
  for (int i = 0; i != len; ++i)
    EEPROM.write(Memory::SSID_BUFFER_START + 1 + i, ssid[i]);

  if (EEPROM.commit() == false)
  {
    Serial.println("Error writing to eeprom.");
    return false;
  }

  s_ssid = ssid;
  return true;
}

bool Network::setPassword(String const &password)
{
  if (!s_initialized) 
  {
    Serial.println("Could not set password. Did you call init()?");
    return false;
  }

  int const len = password.length();
  if (len > Memory::PASSWORD_BUFFER_SIZE)
  {
    Serial.println("Password length exceeds maximum (" + String(Memory::PASSWORD_BUFFER_SIZE) + ")");
    return false;
  }

  EEPROM.write(Memory::PASSWORD_BUFFER_START, len);
  for (int i = 0; i != len; ++i)
    EEPROM.write(Memory::PASSWORD_BUFFER_START + 1 + i, password[i]);

  if (EEPROM.commit() == false)
  {
    Serial.println("Error writing to eeprom.");
    return false;
  }
    
  s_password = password;
  return true;
}

void Network::promptSSID()
{
  Serial.print("Please enter SSID: ");
  while (!Serial.available()) 
  {
    // wait for user to input ssid    
    delay(100);
  }

  String ssid = Serial.readString();
  ssid.trim();
  Serial.println(ssid);
  if (setSSID(ssid))
    Serial.println("SSID set successfully.");
}

void Network::promptPassword()
{
  Serial.print("Please enter password: ");
  while (!Serial.available()) 
  {
    // wait for user to input ssid    
    delay(100);
  }

  String password = Serial.readString();
  password.trim();
  Serial.println(password);
  if (setPassword(password))
    Serial.println("Password set successfully.");
}

bool Network::reset()
{
  EEPROM.write(Memory::GOOD_FLAG_ADDRESS, Memory::GOOD_FLAG_VALUE);
  EEPROM.write(Memory::SSID_BUFFER_START, 0);
  EEPROM.write(Memory::PASSWORD_BUFFER_START, 0);

  s_initialized = true;
  return EEPROM.commit();
}

void Network::fetchSSID()
{
  if (!s_initialized) s_ssid = "";

  byte const len = EEPROM.read(Memory::SSID_BUFFER_START); 

  s_ssid = "";
  for (int i = 0; i != len; ++i)
    s_ssid += static_cast<char>(EEPROM.read(Memory::SSID_BUFFER_START + 1 + i));
}

void Network::fetchPassword()
{
  if (!s_initialized) s_password = "";

  byte const len = EEPROM.read(Memory::PASSWORD_BUFFER_START);

  s_password = "";
  for (int i = 0; i != len; ++i)
    s_password += static_cast<char>(EEPROM.read(Memory::PASSWORD_BUFFER_START + 1 + i));
}
