#include <Wire.h>
#include "SparkFun_VL53L1X.h"

SFEVL53L1X lidarSensor(Wire);

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
// Running people count
char peopleCounterArray[50];

// Size of each location in SPAD array (5x5)
static int ROI_height = 5;
static int ROI_width = 5;

// Path tracking algorithm
static int PathTrack[] = {0, 0, 0, 0};
static int PathTrackFillingSize = 1;
static int LeftPreviousStatus = NOBODY;
static int RightPreviousStatus = NOBODY;

void setup(void)
{
  Wire.begin();
  Serial.begin(9600);
  Serial.println("VL53L1X Lidar People Counter");

  // Configure sensor
  if (lidarSensor.init() == false) {
    Serial.println("Sensor online!");
  }
  lidarSensor.setIntermeasurementPeriod(100);
  lidarSensor.setDistanceModeLong();
  
  delay(1000);
  
  // Calculate threshold amounts for zones
  define_threshold();
}

void loop(void)
{
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

void publishPersonPassage(int zone) {
  if (zone == 1) {
    Serial.println("Out");
  } else if (zone == 2) {
    Serial.println("In");
  } else {
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
          publishPersonPassage(1);
        } else if ((PathTrack[1] == 2)  && (PathTrack[2] == 3) && (PathTrack[3] == 1)) {
          // This an exit
          publishPersonPassage(2);
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
