#include "secrets.h"
#include "WiFiConnector.h"
#include "AWSIoTConnector.h"

WiFiConnector wifi(WIFI_SSID, WIFI_PASSWORD);
AWSIoTConnector aws(AWS_CERT_CA, AWS_CERT_CRT, AWS_CERT_PRIVATE, AWS_IOT_ENDPOINT, AWS_IOT_PUBLISH_TOPIC, THING_NAME);

void setup() {
  Serial.begin(9600);
  wifi.connect();
  aws.connect();
}

void loop() {
  if (!wifi.isConnected()) {
    wifi.connect();
    return;
  }

  if (!aws.isConnected()) {
    aws.connect();
    return;
  }

  aws.publish("{\"hello\":\"world\"}");
  delay(60000);
}
