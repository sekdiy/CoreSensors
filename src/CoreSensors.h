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

#ifndef __CORESENSORS_H__
#define __CORESENSORS_H__

/**
 * CoreSensorsCalibration stores calibration information for temperature and voltage compensation.
 *
 * @see e.g. https://www.avdweb.nl/arduino/measurement/temperature-measurement#h10-calibration
 * @todo create tutorial on calibration
 * @see AVR120: "Characterization and Calibration of the ADC on an AVR"
 * @see AVR122: "Calibration of the AVR's Internal Temperature Reference"
 * @see AVR126: "ADC of megaAVR® in Single-Ended Mode"
 */
class CoreSensorsCalibration
{
public:
  // temperature
  float gainT;          // over intended range
  float offsetT;        // at 0 degree Celsius
  long  lengthT;        // number of samples for averaging

  // voltage
  float gainV;          // over intended range
  float offsetV;        // at nominal supply voltage
  long  lengthV;        // number of samples for averaging
};

/**
 * Pre-defined core sensor calibration datasets.
 */
extern CoreSensorsCalibration const UncalibratedCoreSensors;
extern CoreSensorsCalibration const DuemilanoveCoreSensorsCalibration;
extern CoreSensorsCalibration const ProMiniCoreSensorsCalibration;
extern CoreSensorsCalibration const ProMicroCoreSensorsCalibration;

/**
 * The CoreSensors class.
 */
class CoreSensors
{
public:
  static int const celsius;
  static int const fahrenheit;

  CoreSensors(CoreSensorsCalibration cal = UncalibratedCoreSensors) : calibration(cal) {};
  void begin(CoreSensorsCalibration cal = UncalibratedCoreSensors) { this->calibration = cal; };

  bool update();
  bool updateTemperature();
  bool updateVoltage();

  float getTemperature(bool fahrenheit = false);
  float getVoltage(bool millivolt = false);

  /* deprecated: */
  bool process() { return CoreSensors::update(); }
  bool processTemperature() { return CoreSensors::updateTemperature(); }
  bool processVoltage() { return CoreSensors::updateVoltage(); }

private:
  unsigned long accumulate(unsigned long duration);
  inline unsigned int sample();

  float temperature;
  float voltage;

  CoreSensorsCalibration calibration;
};

/**
 * The CoreSensor object, a singleton that gives access to the core sensors (there is only one set of core sensors!).
 */
extern CoreSensors CoreSensor;

#endif  // __CORESENSORS_H__
