#include "webserver.h"
#include "webpage.h"

WebServer::WebServer(int port):
  d_server(port)
{
  for (int i = 0; i != EVENTLIST_SIZE; ++i)
    d_eventList[i] = EMPTY;
}

void WebServer::begin()
{
  d_server.begin();
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
          int indexVolUp = header.indexOf("GET /volup");
          int indexVolDown = header.indexOf("GET /voldown");
          int indexStop = header.indexOf("GET /stop");

          auto getCounter = [&]() -> int {
            int from = header.indexOf("counter=") + String("counter=").length();
            int to = header.indexOf(" ", from);
            String counter = header.substring(from, to);
            return counter.toInt();
          };

          if (indexVolUp >= 0) 
          {
            int counter = getCounter();
            d_eventList[counter % EVENTLIST_SIZE] = VOLUME_UP_START;
          } 
          else if (indexVolDown >= 0) 
          {
            int counter = getCounter();
            d_eventList[counter % EVENTLIST_SIZE] = VOLUME_DOWN_START;
          }
          else if (indexStop >= 0) 
          {
            int counter = getCounter();
            d_eventList[counter % EVENTLIST_SIZE] = STOP;
          }
     
          // Pass the eventListIndex to the javascript to sync indices
          client.print(pageString);
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

void WebServer::execute()
{
  while (true)
  {
    switch(d_eventList[d_currentEventIndex])
    {
      case EMPTY:
        return;
      case VOLUME_UP_START:
        Serial.println("Volume UP"); 
        break;
      case VOLUME_DOWN_START:
        Serial.println("Volume DOWN");
        break;
      case STOP:
        Serial.println("STOP");
        break;
    }

    d_eventList[d_currentEventIndex++] = EMPTY;
    d_currentEventIndex %= EVENTLIST_SIZE;
  }
}
