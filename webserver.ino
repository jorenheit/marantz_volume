#include "webserver.h"
#include "global_constants.h"

WebServer::WebServer():
  d_server(SERVER_PORT)
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
            execute<Command::VOLUME_UP>(ip);
          } 
          else if (indexVolDn >= 0) 
          {
            execute<Command::VOLUME_DOWN>(ip);
          }
          else if (indexStop >= 0)
          {
            execute<Command::STOP>(ip);
          }
          
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

void WebServer::reset()
{
  d_generator.reset();
  d_activeClientIP = IPAddress{};
  d_currentState = State::IDLE;
}
