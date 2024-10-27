/**
 * This example is intended for evaluating timing requirements and refining calibration profiles.
 */

#include "Arduino.h"
#include "CoreSensors.h"                                                                        // https://github.com/sekdiy/CoreSensors

const long periodDuration = 1000;                                                               // we're setting up a schedule (i.e. every 1000ms)
unsigned long periodTime  = 0;

void setup()
{
  // choose the default calibration profile `CoreSensors::UncalibratedCoreSensors` 
  // or provide a custom profile (see struct `CoreSensorsCalibration` in CoreSensors.h)
  CoreSensor.begin({ 1.0f, 4.5f, 100, 1.0192115269f, 0.0f, 100 });                              // optionally apply sensor calibration
  
  Serial.begin(115200);                                                                         // prepare serial communication
}

void loop()
{
  // let's run the scheduler
  const long currentTime = millis();                                                            // take timestamp of this loop
  const long currentDuration = currentTime - periodTime;                                        // take time difference to last loop

  // if a new scheduled *period* begins
  if (currentDuration >= periodDuration) {

    // take new core temperature measurement (and time it)
    long temperatureDuration = millis();                                                        // take timestamp before
    bool temperatureValid = CoreSensor.updateTemperature();                                     // take temperature measurement
    temperatureDuration = millis() - temperatureDuration;                                       // take timestamp after

    // output temperature measurement result (and time)
    Serial.print(CoreSensor.getTemperature(CoreSensors::celsius), 3); Serial.print(" °C, ");    // print temperature in degree Celsius
    Serial.print(CoreSensor.getTemperature(CoreSensors::fahrenheit), 3); Serial.print(" °F ");  // print temperature in degree Fahrenheit
    Serial.print(temperatureValid ? "(valid, " : "(invalid, ");                                 // print measurement status
    Serial.print("took "); Serial.print(temperatureDuration); Serial.print(" ms), ");           // print measurement duration

    // take new core temperature measurement (and time it)
    long voltageDuration = millis();                                                            // take timestamp before
    bool voltageValid = CoreSensor.updateVoltage();                                             // take voltage measurement
    voltageDuration = millis() - voltageDuration;                                               // take timestamp after

    // output voltage measurement result (and time)
    Serial.print(CoreSensor.getVoltage(), 3); Serial.print(" V ");                              // print voltage
    Serial.print(voltageValid ? "(valid, " : "(invalid, ");                                     // print measurement status
    Serial.print("took "); Serial.print(voltageDuration); Serial.println(" ms)");               // print measurement duration

    // prepare next cycle
    periodTime = currentTime;                                                                   // keep timestamp of this loop
  }

  // There's still time to schedule more events.
  // The actual length of the current loop is currentDuration + temperatureDuration + voltageDuration.
  // So in order to make a schedule with ms precision, we could compensate for this in our periodDuration constant.
  // But keep in mind that this timing behaviour depends on the mcu type and clock.
}