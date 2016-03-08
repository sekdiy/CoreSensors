# CoreSensors Library [![Build Status](https://travis-ci.org/sekdiy/CoreSensors.svg?branch=master)](https://travis-ci.org/sekdiy/CoreSensors)

**CoreSensors** is an Arduino library that supports measuring the AVR's internal temperature and supply voltage.

## What

The **CoreSensors** library allows arduino programs to determine the actual MCU temperature and supply voltage without external components.

## Why

Your arduino might run on battery power.
In that case **CoreSensors** helps to supervise the voltage without having to rely on external components.

Or your project might get hot (or cold).
**CoreSensors** can tell you how hot the Arduino MCU chip gets, independent of sensors in other parts of your project.

## Example

```c++
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
```

This example takes a measurement, displays the core temperature (both in °C and in °F) as well as the core voltage and then repeats.

## How

**CoreSensors** employs two properties that are built into the Arduino:
- an internal substrate [diode](https://en.wikipedia.org/wiki/Diode#Temperature_measurements) that can measure temperature,
- an internal [bandgap reference](https://en.wikipedia.org/wiki/Bandgap_voltage_reference) that works independent of the supply voltage.

The library is aware of different Arduino types, clock speeds and supply voltages. It also features a noise reduction mode and averaging, which improves speed and repeatability.

## Calibration

Since the sensors aren't comparable with high precision external sensors, they can use some individual calibration in order to improve precision.

The **CoreSensors** library provides you with a way to calibrate each individual Arduino (by taking two reference measurements and applying a simple formula, e.g. see [Albert van Dalen](http://goo.gl/Sqmzfs)).
