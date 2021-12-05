#include "Arduino.h"
#include "AWSIoTConnector.h"

AWSIoTConnector::AWSIoTConnector(char* AWS_CERT_CA, char* AWS_CERT_CRT, char* AWS_CERT_PRIVATE, char* publishEndpoint, char* publishTopic, char* thingName)
{
    _AWS_CERT_CA = AWS_CERT_CA;
    _AWS_CERT_CRT = AWS_CERT_CRT;
    _AWS_CERT_PRIVATE = AWS_CERT_PRIVATE;
    _publishEndpoint = publishEndpoint;
    _publishTopic = publishTopic;
    _thingName = thingName;
    _secureWiFi = WiFiClientSecure();
    _client = MQTTClient(256);
}

bool AWSIoTConnector::isConnected() {
    return _client.connected();
}

bool AWSIoTConnector::connect()
{
    _secureWiFi.setCACert(_AWS_CERT_CA);
    _secureWiFi.setCertificate(_AWS_CERT_CRT);
    _secureWiFi.setPrivateKey(_AWS_CERT_PRIVATE);

    _client.begin(_publishEndpoint, 8883, _secureWiFi);

    Serial.print("Connecting to AWS IOT");

    unsigned int startAttemptTime = millis();
    while (!_client.connect(_thingName) && millis() - startAttemptTime < 60000) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();

    if(!_client.connected()){
        Serial.println("AWS IoT Timeout!");
        return false;
    }

    Serial.println("AWS IoT Connected!");
    return true;
}

void AWSIoTConnector::publish(char* json) {
    _client.publish(_publishTopic, json);
    _client.loop();
}
