#include "network.h"

bool Network::s_initialized = false;
String Network::s_ssid = "";
String Network::s_password = "";


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
  return true;
}

void Network::connect()
{
  if (!s_initialized)
    init();

  String ssid = getSSID();
  String password = getPassword();
  
  if (ssid == "")
  {
    Serial.println("Wifi not configured.");
    ssid = promptSSID();
    password = promptPassword();
  }
  else if (password == "")
  {
    Serial.println("No password set for " + ssid + ".");
    password = promptPassword();
  }

  while (true)
  {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
    int const timeout = 5; // seconds
    int count = 0;
    while (WiFi.status() != WL_CONNECTED && count < timeout) 
    {
      delay(1000);
      Serial.print(".");
      ++count;
    }

    if (count != timeout)
      break;

    Serial.println("\nCould not connect to network.");
    ssid = promptSSID();
    password = promptPassword();
  }

  Serial.print("\nWiFi connected.\nIP address: ");
  Serial.println(WiFi.localIP());
}

String const &Network::getSSID()
{
  return s_ssid;
}

String const &Network::getPassword()
{
  return s_password;
}

bool Network::setSSID(String const ssid)
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

bool Network::setPassword(String const password)
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



String Network::promptSSID()
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

  return ssid;
}

String Network::promptPassword()
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

  return password;
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

  byte const ssidLen = EEPROM.read(Memory::SSID_BUFFER_START); 

  s_ssid = "";
  for (int i = 0; i != ssidLen; ++i)
    s_ssid += static_cast<char>(EEPROM.read(Memory::SSID_BUFFER_START + 1 + i));
}

void Network::fetchPassword()
{
  if (!s_initialized) s_password = "";

  byte const passLen = EEPROM.read(Memory::PASSWORD_BUFFER_START);

  s_password = "";
  for (int i = 0; i != passLen; ++i)
    s_password += static_cast<char>(EEPROM.read(Memory::PASSWORD_BUFFER_START + 1 + i));
}
 