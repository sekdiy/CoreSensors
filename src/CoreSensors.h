/**
 * CoreSensors.h
 *
 * An interface to the Arduino's internal temperature (diode reference) and supply voltage (bandgap reference).
 *
 * @author sekdiy (https://github.com/sekdiy/CoreSensors)
 * @date 05.03.2016
 * @version See git comments for changes.
 * @see http://playground.arduino.cc/Main/InternalTemperatureSensor
 */

#ifndef CoreSensors_H
#define CoreSensors_H

typedef struct
{
  float gain;
  float offset;
  long duration;
} SensorCalibration;

extern SensorCalibration proMiniTemperatureDefault;
extern SensorCalibration proMiniVoltageDefault;

extern SensorCalibration duemilanoveTemperatureDefault;
extern SensorCalibration duemilanoveVoltageDefault;

class CoreSensors
{
public:
  CoreSensors(SensorCalibration temperatureCalibration = proMiniTemperatureDefault, SensorCalibration voltageCalibration = proMiniVoltageDefault);

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

  SensorCalibration caliT;
  SensorCalibration caliV;
};

extern CoreSensors CoreSensor;

#endif
