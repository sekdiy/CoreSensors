#include "CoreSensors.h"

void setup() 
{ 
  Serial.begin(115200);
}

void loop() 
{ 
  // update sensor measurements
  CoreSensor.process();

  // output results
  Serial.print(CoreSensor.getTemperature()); Serial.print(" degC, ");
  Serial.print(CoreSensor.getTemperature(true)); Serial.print(" degF, ");
  Serial.print(CoreSensor.getVoltage()); Serial.println(" V");

  delay(1000);
}

