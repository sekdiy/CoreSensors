# CoreSensors Library [![Version](https://img.shields.io/badge/CoreSensors-0.2.0-blue.svg 'still in beta')](https://github.com/sekdiy/CoreSensors) [![Build Status](https://travis-ci.org/sekdiy/CoreSensors.svg?branch=master)](https://travis-ci.org/sekdiy/CoreSensors) [![analytics](http://www.google-analytics.com/collect?v=1&t=pageview&dl=https%3A%2F%2Fgithub.com%2Fsekdiy%2FCoreSensors&cid=2238D739-76DE-4205-9768-2F3277FA2561&tid=UA-65656434-2&aip=1&dt=README)]()

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

A more complex example is also provided with the library. It demonstrates scheduling and processing the two sensors independently.

## How

**CoreSensors** employs two properties that are built into the Arduino:
- an internal substrate [diode](https://en.wikipedia.org/wiki/Diode#Temperature_measurements) that can measure temperature,
- an internal [bandgap reference](https://en.wikipedia.org/wiki/Bandgap_voltage_reference) that works independent of the supply voltage.

The library is aware of different Arduino types, clock speeds and supply voltages. It also features a noise reduction mode and averaging, which improves speed and repeatability.

## Calibration

Since the sensors aren't comparable with high precision external sensors, they can use some individual calibration in order to improve precision.

The **CoreSensors** library provides you with a way to calibrate each individual Arduino (by taking two reference measurements and applying a simple formula, e.g. see [Albert van Dalen](http://goo.gl/Sqmzfs) and [Oregon Embedded](http://oregonembedded.com/slopeoffset.html)).

## Note

This library defines a dummy ADC_vect interrupt service handler routine.
In case your code requires its own ADC_vect routine, you can simply replace the one in this library without any negative impact.
But please keep in mind that the order of include statements in your code might influence which ADC_vect routine will actually be applied.