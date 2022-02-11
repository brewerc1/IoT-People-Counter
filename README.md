# IoT-People-Counter

## Electronics

The IoT Solution cosists of two components:

- ESP32
- VL53L1X Lidar Sensor

Data is published to AWS IoT Core via MQTT using our WiFiConnector & AWSIoTConnector Libraries

An IoT Rule automatically moves messages recieved in IoT to SQS for processing

Written using Arduino (C++)

## Backend
