/*
  ESP32:
  SDA -> GPIO21
  SCL -> GPIO22
  VCC -> 5V
  GND -> GND
*/

#include <Wire.h>
#include "SparkFun_VL53L1X.h"
#include "secrets.h"
#include "WiFiConnector.h"
#include "AWSIoTConnector.h"

// Zone states
static int NOBODY = 0;
static int SOMEONE = 1;

// Zone names
static int LEFT = 0;
static int RIGHT = 1;

// Threshold to consider an entrance, exit
static int DIST_THRESHOLD_MAX[] = {0, 0};

// SPAD array location for the zones
static int center[2] = {239, 175};

// Current zone
static int Zone = 0;

// Size of each location in SPAD array (5x5)
static int ROI_height = 5;
static int ROI_width = 5;

// Path tracking algorithm
static int PathTrack[] = {0, 0, 0, 0};
static int PathTrackFillingSize = 1;
static int LeftPreviousStatus = NOBODY;
static int RightPreviousStatus = NOBODY;

// Sensor object
SFEVL53L1X lidarSensor(Wire);

// Current count
// Needed by both tasks
static int peopleInRoom = 0;

// Two tasks
TaskHandle_t PeopleCounterTaskHandler;
TaskHandle_t PublishDataTaskHandler;

void setup(void)
{
  Serial.begin(9600);
  // Set up people counting on core 0
  xTaskCreatePinnedToCore(
    peopleCounterTaskFunction,
    "PeopleCounterTask",
    10000,
    NULL,
    1,
    &PeopleCounterTaskHandler,
    0
  );
  // Set up data publishing on core 1
  xTaskCreatePinnedToCore(
    publishDataTaskFunction,
    "PublishDataTask",
    10000,
    NULL,
    1,
    &PublishDataTaskHandler,
    1
  );
}

// When using both cores, they each have their own "loop" function inside the task, this is not needed.
void loop(void) {}

// Runs repeatedely on core 0 - similar to loop, setup without multithreading
void peopleCounterTaskFunction(void * pvParameters) {
  // Setup
  Serial.println("People Counter Running On Core 0");
  // Configure sensor
  Wire.begin();
  if (lidarSensor.init() == false) {
    Serial.println("Sensor online!");
  }
  lidarSensor.setIntermeasurementPeriod(100);
  lidarSensor.setDistanceModeLong();

  delay(1000);
  // Calculate threshold amounts for zones
  define_threshold();
  // Loop
  while (1) {
    uint16_t distance;
    lidarSensor.setROI(ROI_height, ROI_width, center[Zone]);  // first value: height of the zone, second value: width of the zone
    delay(50);

    lidarSensor.setTimingBudgetInMs(50);
    lidarSensor.startRanging(); //Write configuration bytes to initiate measurement
    distance = lidarSensor.getDistance(); //Get the result of the measurement from the sensor
    lidarSensor.stopRanging();

    // inject the new ranged distance in the people counting algorithm
    processPeopleCountingData(distance, Zone);
    Zone++;
    Zone = Zone % 2;
  }
}

// Runs repeatedly on core 1 
void publishDataTaskFunction(void * pvParameters) {
  // Setup
  Serial.println("Data publisher running on core 1");
  WiFiConnector wifi(WIFI_SSID, WIFI_PASSWORD);
  AWSIoTConnector aws(AWS_CERT_CA, AWS_CERT_CRT, AWS_CERT_PRIVATE, AWS_IOT_ENDPOINT, AWS_IOT_PUBLISH_TOPIC, THING_NAME);
  wifi.connect();
  aws.connect();
  // Loop
  while (1) {
    // Wait 5 minutes before publishing data
     delay(300000);
    // Build json
    String json = "{\"total\":";
    json += String(peopleInRoom);
    json += ",";
    json += "\"device\": \"";
    json += DEVICE_ID;
    json += "\"";
    json += "}";
    // Move json to buffer
    unsigned int len = 50;
    char jsonBuffer[len];
    json.toCharArray(jsonBuffer, len);
    // Publish
    // Reconnect if the connection was lost
    if (!wifi.isConnected())wifi.connect();
    if (!aws.isConnected())aws.connect();
    aws.publish(jsonBuffer);
    Serial.println(json);
  }
}

// Utility functions
void define_threshold() {
  delay(500);
  Zone = 0;
  float sum_zone_0 = 0;
  float sum_zone_1 = 0;
  uint16_t distance;
  int number_attempts = 100;
  for (int i = 0; i < number_attempts; i++) {
    // increase sum of values in Zone 0
    lidarSensor.setROI(ROI_height, ROI_width, center[Zone]);  // first value: height of the zone, second value: width of the zone
    delay(50);
    lidarSensor.setTimingBudgetInMs(50);
    lidarSensor.startRanging(); //Write configuration bytes to initiate measurement
    distance = lidarSensor.getDistance(); //Get the result of the measurement from the sensor
    lidarSensor.stopRanging();
    sum_zone_0 = sum_zone_0 + distance;
    Zone++;
    Zone = Zone % 2;

    // increase sum of values in Zone 1
    lidarSensor.setROI(ROI_height, ROI_width, center[Zone]);  // first value: height of the zone, second value: width of the zone
    delay(50);
    lidarSensor.setTimingBudgetInMs(50);
    lidarSensor.startRanging(); //Write configuration bytes to initiate measurement
    distance = lidarSensor.getDistance(); //Get the result of the measurement from the sensor
    lidarSensor.stopRanging();
    sum_zone_1 = sum_zone_1 + distance;
    Zone++;
    Zone = Zone % 2;
  }
  // after we have computed the sum for each zone, we can compute the average distance of each zone
  float average_zone_0 = sum_zone_0 / number_attempts;
  float average_zone_1 = sum_zone_1 / number_attempts;

  float threshold_zone_0 = average_zone_0 * 80 / 100;
  float threshold_zone_1 = average_zone_1 * 80 / 100;

  DIST_THRESHOLD_MAX[0] = threshold_zone_0;
  DIST_THRESHOLD_MAX[1] = threshold_zone_1;
  delay(2000);
}

void handlePersonPassage(int zone) {
  int change;
  if (zone == 1) {
    Serial.println("Out");
    change = -1;
    peopleInRoom--;
  } else if (zone == 2) {
    Serial.println("In");
    change = 1;
    peopleInRoom++;
  } else {
    change = 0;
    Serial.println("Error, Unknown zone!");
  }
}

// People Counting algorithm
void processPeopleCountingData(int16_t Distance, uint8_t zone) {
  int CurrentZoneStatus = NOBODY;
  int AllZonesCurrentStatus = 0;
  int AnEventHasOccured = 0;

  if (Distance < DIST_THRESHOLD_MAX[Zone]) {
    // Someone is in !
    CurrentZoneStatus = SOMEONE;
  }

  // left zone
  if (zone == LEFT) {

    if (CurrentZoneStatus != LeftPreviousStatus) {
      // event in left zone has occured
      AnEventHasOccured = 1;

      if (CurrentZoneStatus == SOMEONE) {
        AllZonesCurrentStatus += 1;
      }
      // need to check right zone as well ...
      if (RightPreviousStatus == SOMEONE) {
        // event in left zone has occured
        AllZonesCurrentStatus += 2;
      }
      // remember for next time
      LeftPreviousStatus = CurrentZoneStatus;
    }
  }
  // right zone
  else {

    if (CurrentZoneStatus != RightPreviousStatus) {

      // event in left zone has occured
      AnEventHasOccured = 1;
      if (CurrentZoneStatus == SOMEONE) {
        AllZonesCurrentStatus += 2;
      }
      // need to left right zone as well ...
      if (LeftPreviousStatus == SOMEONE) {
        // event in left zone has occured
        AllZonesCurrentStatus += 1;
      }
      // remember for next time
      RightPreviousStatus = CurrentZoneStatus;
    }
  }

  // if an event has occured
  if (AnEventHasOccured) {
    if (PathTrackFillingSize < 4) {
      PathTrackFillingSize ++;
    }

    // if nobody anywhere lets check if an exit or entry has happened
    if ((LeftPreviousStatus == NOBODY) && (RightPreviousStatus == NOBODY)) {

      // check exit or entry only if PathTrackFillingSize is 4 (for example 0 1 3 2) and last event is 0 (nobobdy anywhere)
      if (PathTrackFillingSize == 4) {
        // check exit or entry. no need to check PathTrack[0] == 0 , it is always the case
        if ((PathTrack[1] == 1)  && (PathTrack[2] == 3) && (PathTrack[3] == 2)) {
          // this is an entry
          handlePersonPassage(1);
        } else if ((PathTrack[1] == 2)  && (PathTrack[2] == 3) && (PathTrack[3] == 1)) {
          // This an exit
          handlePersonPassage(2);
        }
      }
      for (int i = 0; i < 4; i++) {
        PathTrack[i] = 0;
      }
      PathTrackFillingSize = 1;
    }
    else {
      // update PathTrack
      // example of PathTrack update
      // 0
      // 0 1
      // 0 1 3
      // 0 1 3 1
      // 0 1 3 3
      // 0 1 3 2 ==> if next is 0 : check if exit
      PathTrack[PathTrackFillingSize - 1] = AllZonesCurrentStatus;
    }
  }
}
