#include "webserver.h"

WebServer::WebServer(int port):
  d_server(port)
{}

void WebServer::begin()
{
  d_server.begin();
  d_generator.start();
}

bool WebServer::handle()
{
  WiFiClient client = d_server.available();

  if (!client)
    return false;
                  
  unsigned long currentTime = millis();
  unsigned long previousTime = currentTime;
  unsigned long const timeoutTime = 2000;

  String currentLine = "";
  String header = "";

  while (client.connected() && currentTime - previousTime <= timeoutTime) 
  {
    currentTime = millis();
    if (client.available()) 
    {
      char const c = client.read();
      header += c;
      
      if (c == '\n')
      {                    
        if (currentLine.length() == 0) 
        {
          int indexVolUp = header.indexOf(String("GET ") +  d_volumeUpUrl);
          int indexVolDn = header.indexOf(String("GET ") +  d_volumeDnUrl);
          int indexStop  = header.indexOf(String("GET ") +  d_stopUrl);

          IPAddress const ip = client.remoteIP();

          if (indexVolUp >= 0) 
          {
            Serial.println("Volume UP");
            volumeUp(ip);
          } 
          else if (indexVolDn >= 0) 
          {
            Serial.println("Volume DOWN");
            volumeDown(ip);
          }
          else if (indexStop >= 0)
          {
            Serial.println("STOP");
            volumeStop(ip);
          }
          else
            client.print(s_webpage);

          break;
        } 
        else
          currentLine = "";
      } 
      else if (c != '\r')
        currentLine += c;
    }
  }

  // Close the connection
  client.stop();
  return true;
}

void WebServer::volumeUp(IPAddress const &clientIP)
{
  switch (d_currentState)
  {
    case State::IDLE:
    {
      d_generator.schedule<RC5::VolumeUp>();
      d_activeClientIP = clientIP;
      d_currentState = State::VOLUME_UP;
      return;
    }
    case State::VOLUME_CHANGE_STOP:
    {
      // Server already in stopped state, apparently the 
      // stop signal arrived before the action-signal. 
      // Ignore and set state to IDLE.

      if (clientIP == d_activeClientIP)
      {
        d_currentState = State::IDLE;
      }
      return;
    }
    // In all other cases: ignore this signal
    default: return;
  }
}

void WebServer::volumeDown(IPAddress const &clientIP)
{
  switch (d_currentState)
  {
    case State::IDLE:
    {
      d_generator.schedule<RC5::VolumeDown>();
      d_activeClientIP = clientIP;
      d_currentState = State::VOLUME_DOWN;
      return;
    }
    case State::VOLUME_CHANGE_STOP:
    {
      // Server already in stopped state; apparently the 
      // stop signal arrived before the action-signal. Schedule non-repeating
      // signal and set state to IDLE.

      if (clientIP == d_activeClientIP)
      {    
        d_currentState = State::IDLE;
      }
        
      return;
    }
    // In all other cases: ignore this signal
    default: return;
  }
}

void WebServer::volumeStop(IPAddress const &clientIP)
{
  switch (d_currentState)
  {
    case State::IDLE:
    {
      // Stop signal arrived before action. Do nothing now but change
      // state to STOP. The next action signal from this client will
      // be executed once immediately.
      d_activeClientIP = clientIP;
      d_currentState = State::VOLUME_CHANGE_STOP;
      return;
    }
    default:
    {
      if (clientIP == d_activeClientIP) // only listen to stop command from the active client
      {
        d_generator.schedule<RC5::None>();
        d_currentState = State::IDLE;
      }
      return;
    }
  }
}
