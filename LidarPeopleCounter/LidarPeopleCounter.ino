// Current issues: Person standing in the doorway gets counted when they finally move (direction is usually correct)
// They should not be counted at all: what if somebody stands in the door and moves in repeatadley? Then they inflate the in count
// Need to somehow set a flag

#include "VL53L1X_ULD.h"

#define THRESHOLD_AMOUNT 3

// Sensor obj and sensor status
VL53L1X_ULD sensor;
VL53L1_Error status;

#define MIN_HEIGHT 1370 // (~4ft 6in)

// Center in SPAD array (167  = left center, 231 = right center)
int center[2] = { 167, 231 };

// Baseline distance for sensor, calibrated in setup
// Long to avoid overflow during calibration
long baseline[2];
// How close you need to be to trigger (so we don't count something like a chair as a person)
// distance must be < threshold to trigger
int thresholds[2];

// 0 = left, 1 = right
// Swapped during each iteration
int zone = 0;
// Buffers for each zone
#define BUFFER_LENGTH 15
int lBuffer[BUFFER_LENGTH];
int rBuffer[BUFFER_LENGTH];

// Measured distance, status
uint16_t distance;
ERangeStatus rangeStatus;

void setup() {
  Serial.begin(115200);
  initializeSensor();
  configureSensor();
  calibrateBaseline();
  flushBuffers();
}

void loop() {
  uint8_t dataReady = false;
  while (!dataReady) {
    sensor.CheckForDataReady(&dataReady);
    delay(5);
  }

  sensor.GetRangeStatus(&rangeStatus);
  sensor.GetDistanceInMm(&distance);
  sensor.ClearInterrupt();

  // Populate buffers
  if (zone == 0) {
    rightRotate(lBuffer, BUFFER_LENGTH);
    lBuffer[0] = distance;
  } else {
    rightRotate(rBuffer, BUFFER_LENGTH);
    rBuffer[0] = distance;
  }

  checkForPeople();

  zone++;
  zone = zone % 2;
  sensor.SetROICenter(center[zone]);
}

void checkForPeople() {
  uint8_t firstL = -1; // Index of first occurence in left buffer
  uint8_t firstR = -1; // Index of first occurence in right buffer

  uint8_t consL = 0; // Consecutive threshold occurences in left buffer
  uint8_t consR = 0; // Consecutive threshold occurences in right buffer

  bool lastTL = false; // Was the last reading below threshold in left buffer?
  bool lastTR = false; // Was the last reading below threshold in right buffer?


  // TODO: abstract into function to avoid copy paste
  for (uint8_t i = 0; i < BUFFER_LENGTH; i++) {
    if (consL >= THRESHOLD_AMOUNT) {
      if (lBuffer[i] < thresholds[0]) {
        consL++;
        lastTL = true;
        continue;
      } else {  if (firstL != 0) break; } 
    }

    if (lBuffer[i] < thresholds[0]) {
      if (!lastTL) {
        firstL = i;
      }
      lastTL = true;
      consL++;
    } else {
      lastTL = false;
      consL = 0;
    }
  }

  // See above TODO
  for (uint8_t i = 0; i < BUFFER_LENGTH; i++) {
    if (consR >= THRESHOLD_AMOUNT) {
      if (rBuffer[i] < thresholds[1]) {
        consR++;
        lastTR = true;
        continue;
      } else { if (firstR != 0) break; } 
    }
    
    if (rBuffer[i] < thresholds[1]) {
      if (!lastTR) {
        firstR = i;
      }
      lastTR = true;
      consR++;
    } else {
      lastTR = false;
      consR = 0;
    }
  }

  if (consL == BUFFER_LENGTH && consR == BUFFER_LENGTH) { 
    Serial.println("Person standing in the doorway, move!");
    flushBuffers();
    return;  
  }

  if (consL >= THRESHOLD_AMOUNT && consR >= THRESHOLD_AMOUNT) {
    if (firstL < firstR) {
      Serial.println("Out");
    } else if (firstR < firstL) {
      Serial.println("In");
    } else {
      // Detected at the same time
      // TODO: figure out how to handle this
      // IDEA: one with smaller # over 3 consec vals is the dir it was coming from
      int lTotal = 0; int rTotal = 0;
      for (uint8_t i = firstL; i < firstL + THRESHOLD_AMOUNT; i++) {
        lTotal += lBuffer[i];
        rTotal += rBuffer[i];
      }
      lTotal /= THRESHOLD_AMOUNT; rTotal /= THRESHOLD_AMOUNT;
      if (lTotal < rTotal) {
        Serial.println("Out");
      } else {
        Serial.println("In");
      }
    }
    flushBuffers();
  }
}

void initializeSensor() {
  // Initialize the I2C controller
  Wire.begin();
  // Initialize the sensor
  status = sensor.Begin();
  if (status != VL53L1_ERROR_NONE) {
    Serial.println("Could not initialize the sensor, error code: " + String(status));
    while (1) {}
  }
  Serial.println("Sensor initialized");
}

void configureSensor() {
  sensor.Init();
  sensor.SetDistanceMode(2);
  sensor.SetTimingBudgetInMs(20); 
  sensor.SetInterMeasurementInMs(21); 
  sensor.SetROI(8, 16); // 8x16 zones
  sensor.SetROICenter(center[zone]); // Start on the left (167)
  sensor.StartRanging(); // Start measuring
}

void outputResults() {
  Serial.print("Ctr = ");
  Serial.print(String(center[zone]));
  Serial.print(", s - ");
  Serial.print(String(rangeStatus));
  Serial.print(", d = ");
  Serial.println(String(distance));
}

// Loops 100 times to get an accurate baseline distance for calculations
void calibrateBaseline() {
  for (int i = 0; i < 100; i++) {
    uint8_t dataReady = false;
    while (!dataReady) {
      sensor.CheckForDataReady(&dataReady);
      delay(5);
    }

    sensor.GetDistanceInMm(&distance);
    sensor.ClearInterrupt();

    // Add measured distance to baseline
    baseline[zone] += distance;

    zone++;
    zone = zone % 2;

    sensor.SetROICenter(center[zone]);
  }
  // Take average of each measurement to use for baseline
  baseline[0] = baseline[0] / 50;
  baseline[1] = baseline[1] / 50;

  thresholds[0] = baseline[0] - MIN_HEIGHT;
  thresholds[1] = baseline[1] - MIN_HEIGHT;

  Serial.print("Baseline measurements: L=");
  Serial.print(String(baseline[0]));
  Serial.print(", R=");
  Serial.print(String(baseline[1]));
  Serial.println();
}

void rightRotate(int* arr, int len)
{
  for (int i = len - 1; i > 0; i--) {
    arr[i] = arr[i - 1];
  }
  arr[0] = 0;
}

void flushBuffers() {
  for (int i = 0; i < BUFFER_LENGTH; i++) {
    lBuffer[i] = 9999;
  }
}
