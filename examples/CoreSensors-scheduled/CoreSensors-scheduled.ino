#include "CoreSensors.h"    // https://github.com/sekdiy/CoreSensors

const long period = 1000;
long lastTime = 0;
long processDuration = 0;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  const long currentTime = millis();
  const long currentDuration = currentTime - lastTime;

  if (currentDuration >= period) {

    // take new core temperature measurements (and time it)
    processDuration = millis();
    CoreSensor.processTemperature();
    processDuration = millis() - processDuration;

    // output temperature measurement results
    Serial.print(CoreSensor.getTemperature(), 4); Serial.print(" degC, ");
    Serial.print(CoreSensor.getTemperature(true), 4); Serial.print(" degF ");
    Serial.print("(took "); Serial.print(processDuration); Serial.print(" ms), ");

    // take new core voltage measurements (and time it)
    processDuration = millis();
    CoreSensor.processVoltage();
    processDuration = millis() - processDuration;

    // output voltage measurement results
    Serial.print(CoreSensor.getVoltage(), 4); Serial.print(" V ");
    Serial.print("(took "); Serial.print(processDuration); Serial.println(" ms)");

    // prepare for next cycle
    lastTime = currentTime;
  }

  // there's still time to do more...
}
