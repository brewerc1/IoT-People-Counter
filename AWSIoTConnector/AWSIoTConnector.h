/* Simple library to connect & publish to AWS IoT */

#ifndef AWSIoTConnector_h
#define AWSIoTConnector_h

#include "Arduino.h"
#include "WiFiClientSecure.h"
#include "MQTTClient.h"

class AWSIoTConnector
{
private:
    char* _AWS_CERT_CA;
    char* _AWS_CERT_CRT;
    char* _AWS_CERT_PRIVATE;
    char* _publishEndpoint;
    char* _publishTopic;
    char* _thingName;
    WiFiClientSecure _secureWiFi;
    MQTTClient _client;
public:
    AWSIoTConnector(char* AWS_CERT_CA, char* AWS_CERT_CRT, char* AWS_CERT_PRIVATE, char* publishEndpoint, char* publishTopic, char* thingName);
    bool connect();
    bool isConnected();
    void publish(char* json);
};

#endif
