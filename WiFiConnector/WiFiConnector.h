/* Simple library to connect to 2.4ghz wifi with ssid & password */

#ifndef WiFiConnector_h
#define WiFiConnector_h

#include "Arduino.h"
#include "WiFi.h"
class WiFiConnector
{
private:
  char* _ssid;
  char*  _password;
public:
    WiFiConnector(char* ssid, char* password);
    bool connect();
    bool isConnected();
};

#endif
