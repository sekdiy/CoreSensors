#include "Arduino.h"
#include "CoreSensors.h"    // https://github.com/sekdiy/CoreSensors

void setup() 
{ 
  Serial.begin(115200);
}

void loop() 
{ 
  // update sensor measurements
  CoreSensor.update();

  // output results
  Serial.print(CoreSensor.getTemperature()); Serial.print(" degC, ");
  Serial.print(CoreSensor.getTemperature(true)); Serial.print(" degF, ");
  Serial.print(CoreSensor.getVoltage()); Serial.println(" V");

  // wait a second
  delay(1000);
}

