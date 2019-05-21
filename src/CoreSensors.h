/**
 * CoreSensors.h
 *
 * An interface to the Arduino's internal temperature (diode reference) and supply voltage (bandgap reference).
 *
 * @author sekdiy (https://github.com/sekdiy/CoreSensors)
 * @date 08.03.2016
 * @version See git comments for changes.
 * @see http://playground.arduino.cc/Main/InternalTemperatureSensor
 */

#ifndef CoreSensors_H
#define CoreSensors_H

/**
 * The CoreSensorsCalibration structure stores calibration information for temperature and voltage compensation.
 *
 * Arduino Pro Mini:    { 1.22f, 64.85f, 1000, 1.0192115269f, 0.0f, 1000 }
 * Arduino Duemilanove: { 1.1786564159f, 48.8f, 1000, 1.0261748959f, 0.0f, 1000 }
 *
 * @see e.g. http://goo.gl/Sqmzfs
 * @todo create tutorial on calibration
 */
typedef struct
{
  // temperature
  float gainT;          // over intended range
  float offsetT;        // at 0 degree Celsius
  long  lengthT;        // number of samples for averaging

  // voltage
  float gainV;          // over intended range
  float offsetV;        // at nominal supply voltage
  long  lengthV;        // number of samples for averaging
} CoreSensorsCalibration;

/**
 * The CoreSensors class.
 */
class CoreSensors
{
public:
  static int const celsius;
  static int const fahrenheit;

  static CoreSensorsCalibration const UncalibratedCoreSensors;

  CoreSensors() : calibration(UncalibratedCoreSensors) {};

  void begin(CoreSensorsCalibration calibration);

  bool process();
  bool processTemperature();
  bool processVoltage();

  float getTemperature(bool fahrenheit = false);
  float getVoltage();

private:
  unsigned long accumulate(unsigned long duration);
  inline unsigned int sample();

  float temperature;
  float voltage;

  CoreSensorsCalibration calibration;
};

/**
 * The CoreSensor object, a singleton that gives access to the core sensors (there is only one AVR core!).
 */
extern CoreSensors CoreSensor;

#endif
