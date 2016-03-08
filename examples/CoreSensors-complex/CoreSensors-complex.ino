#include "CoreSensors.h"                        // https://github.com/sekdiy/CoreSensors

const long periodDuration = 1000;               // we're setting up a schedule (i.e. every 1000ms)
unsigned long periodTime  = 0;

void setup()
{
  CoreSensor.begin({ 1.22f, 64.85f, 1000, 1.0192115269f, 0.0f, 1000 }); // apply sensor calibration
  Serial.begin(115200);
}

void loop()
{
  // let's run the scheduler
  const long currentTime = millis();
  const long currentDuration = currentTime - periodTime;

  // if the scheduled *period* is over
  if (currentDuration >= periodDuration) {

    // take new core temperature and voltage measurements (and time it)
    long processDuration = millis();
    Serial.print(CoreSensor.process() ? "Valid core sensor data: " : "Invalid (probably due to lack of calibration): ");
    processDuration = millis() - processDuration;

    // output temperature measurement results
    Serial.print(CoreSensor.getTemperature(), 4); Serial.print(" degC, ");
    Serial.print(CoreSensor.getTemperature(true), 4); Serial.print(" degF ");

    // output voltage measurement results
    Serial.print(CoreSensor.getVoltage(), 4); Serial.print(" V ");

    // output processing duration
    Serial.print("(took "); Serial.print(processDuration); Serial.println(" ms)");

    // prepare next cycle
    periodTime = currentTime;
  }

  // there's still time to schedule more events...
}
