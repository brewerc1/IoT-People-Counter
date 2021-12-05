#include "Arduino.h"
#include "WiFiConnector.h"

WiFiConnector::WiFiConnector(char* ssid, char* password)
{
    _ssid = ssid;
    _password = password;
}

bool WiFiConnector::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiConnector::connect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    Serial.print("Connecting");

    unsigned int startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 60000) {
        Serial.print(".");
        delay(500);
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to wifi");
        return false;
    }

    Serial.println();
    Serial.println("Connected!");

    return true;
}
