#include "webserver.h"

WebServer::WebServer(int port):
  d_server(port)
{}

void WebServer::begin()
{
  d_server.begin();
}

bool WebServer::handle(RC5::Generator &generator)
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

          if (indexVolUp >= 0) 
          {
            bool toggle = (header.indexOf(d_volumeUpUrl + String("hold")) < 0);
            generator.push<RC5::VolUp>(toggle);
           
            if (toggle)
              Serial.println("Volume UP");
            else
              Serial.println("Hold");
          } 
          else if (indexVolDn >= 0) 
          {
            bool toggle = (header.indexOf(d_volumeDnUrl + String("hold")) < 0);
            generator.push<RC5::VolDn>(toggle);
            if (toggle)
               Serial.println("Volume DOWN");
            else
              Serial.println("Hold");
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
