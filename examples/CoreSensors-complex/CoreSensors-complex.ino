#include "CoreSensors.h"                                                              // https://github.com/sekdiy/CoreSensors

const long periodDuration = 1000;                                                     // we're setting up a schedule (i.e. every 1000ms)
unsigned long periodTime  = 0;

void setup()
{
  CoreSensor.begin({ 1.22f, 64.85f, 1000, 1.0192115269f, 0.0f, 1000 });               // apply sensor calibration
  Serial.begin(115200);                                                               // prepare serial communication
}

void loop()
{
  // let's run the scheduler
  const long currentTime = millis();                                                  // take timestamp of this loop
  const long currentDuration = currentTime - periodTime;                              // take time difference to last loop

  // if a new scheduled *period* begins
  if (currentDuration >= periodDuration) {

    // take new core temperature measurement (and time it)
    long temperatureDuration = millis();                                              // take timestamp before
    bool temperatureValid = CoreSensor.processTemperature();                          // take temperature measurement
    temperatureDuration = millis() - temperatureDuration;                             // take timestamp after

    // output temperature measurement result (and time)
    Serial.print(CoreSensor.getTemperature(), 4); Serial.print(" degC, ");            // print temperature in degree Celsius
    Serial.print(CoreSensor.getTemperature(true), 4); Serial.print(" degF ");         // print temperature in degree Fahrenheit
    Serial.print(temperatureValid ? "(valid, " : "(invalid, ");                       // print measurement status
    Serial.print("took "); Serial.print(temperatureDuration); Serial.print(" ms), "); // print measurement duration

    // take new core temperature measurement (and time it)
    long voltageDuration = millis();                                                  // take timestamp before
    bool voltageValid = CoreSensor.processVoltage();                                  // take voltage measurement
    voltageDuration = millis() - voltageDuration;                                     // take timestamp after

    // output voltage measurement result (and time)
    Serial.print(CoreSensor.getVoltage(), 4); Serial.print(" V ");                    // print voltage
    Serial.print(temperatureValid ? "(valid, " : "(invalid, ");                       // print measurement status
    Serial.print("took "); Serial.print(voltageDuration); Serial.println(" ms)");     // print measurement duration

    // prepare next cycle
    periodTime = currentTime;                                                         // keep timestamp of this loop
  }

  // There's still time to schedule more events.
  // The actual length of the current loop is currentDuration + temperatureDuration + voltageDuration.
  // So in order to make a schedule with ms precision, we could compensate for this in our periodDuration constant.
  // But keep in mind that this timing behaviour depends on the mcu type and clock.
}
