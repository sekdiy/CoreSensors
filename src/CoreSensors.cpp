/**
 * CoreSensors.cpp
 *
 * An interface to the Arduino's internal temperature (diode reference) and supply voltage (bandgap reference).
 *
 * @author sekdiy (https://github.com/sekdiy/CoreSensors)
 * @date 05.03.2016
 * @version See git comments for changes.
 * @see http://playground.arduino.cc/Main/InternalTemperatureSensor
 */

#include <avr/sleep.h>      // built-in AVR sleep library
#include "Arduino.h"        // we're no longer compatible to IDEs < 1.0
#include "CoreSensors.h"    // https://github.com/sekdiy/CoreSensors

SensorCalibration duemilanoveTemperatureDefault = { 1.1786564159f, 321.95f, 1000 };     // 328P, see: http://www.netquote.it/nqmain/arduino-nano-v3-internal-temperature-sensor/
// uncalibrated 0°C: 321,95K/273,15K = 1,1786564159
// calibrated 0°C: -0,8°C +/- 0.3K
SensorCalibration proMiniTemperatureDefault = { 1.22, 338.0f, 1000 };                   // Pro Mini 3,3V@5V
// uncalibrated 0°C: 8,18°C -> 331.18K
// offset’ = deciCelsius * gain / 10 + offset – gain * T (see: http://www.avdweb.nl/arduino/hardware-interfacing/temperature-measurement.html#h10-calibration)
SensorCalibration duemilanoveVoltageDefault = { 1.0261748959f, 0.0f, 1000 };            // Duemilanove
SensorCalibration proMiniVoltageDefault = { 1.0192115269f, 0.0f, 1000 };                // Pro Mini 3,3V@5V

/**
 * Default constructor, sets calibration data for both temperature and voltage correction.
 */
CoreSensors::CoreSensors(SensorCalibration temperatureCalibration, SensorCalibration voltageCalibration) : caliT(temperatureCalibration), caliV(voltageCalibration) {};

/**
 * Process both temperature and voltage measurements and store results.
 */
bool CoreSensors::process()
{
    return (this->processTemperature() && this->processVoltage());
}

/**
 * Process temperature measurement and store result.
 */
bool CoreSensors::processTemperature()
{
    // disable interrupts
    cli();

    // save old mux setting
    byte mux = ADMUX;

    // select internal temperature sensor
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega168A__) ||  defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328__) ||  defined(__AVR_ATmega328P__)
    // ATmega 32U4: Arduino Leonardo and compatible
    // ATmega 168A/P:  Arduino Decimilia and older
    // ATmega 328(P): Arduino Duemilanove, Uno and compatible
    ADMUX = bit(REFS1) | bit(REFS0) |  bit(MUX3);
#else
    // ATmega8 : No
    // ATmega8L : No
    // ATmega8A : No
    // ATmega168 : No
    // ATmega1280 (Arduino Mega) : No
    // ATmega2560 (Arduino Mega 2560) : No
    // please contact me if you know the specifics of other MCUs
    #error unsupported MCU
#endif

    // enable interrupts
    sei();

    // wait for the reference to stabilize
    delay(20);  // TODO determine optimal value

    // store temperature
    this->temperature = (float) ((this->accumulate(this->caliT.duration) / float(this->caliT.duration) - this->caliT.offset) / this->caliT.gain);

    // restore old mux setting
    ADMUX = mux;

    // check if temperature is valid
    return (-100 < this->temperature);
}

/**
 * Process voltage measurement and store result.
 */

bool CoreSensors::processVoltage()
{
    // disable interrupts
    cli();

    // save old mux setting
    byte mux = ADMUX;

//   // use the 1.1 V internal reference
// #if defined(__AVR_ATmega2560__)
//    analogReference(INTERNAL1V1);
// #else
//    analogReference(INTERNAL);
// #endif

    // select internal band gap reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    // ATmega 32U4: Arduino Leonardo and compatible
    // ATmega 1280: Arduino Mega and compatible
    // ATmega 2560: Arduino Mega 2560 and compatible
    ADMUX =  _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATmega168__) ||  defined(__AVR_ATmega328__) || defined(__AVR_ATmega168P__) ||  defined(__AVR_ATmega328P__)
    // ATmega 168:  Arduino Decimilia and older
    // ATmega 328P: Arduino Duemilanove, Uno and compatible
    ADMUX =  _BV(REFS0) | _BV(MUX3) | _BV(MUX2) |_BV (MUX1);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    // ATTinyX5: various types
    ADMUX = _BV(MUX3) | _BV(MUX2);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    // ATTinyX4: various types
    ADMUX =  _BV(MUX5) | _BV(MUX0);
#else
    // please contact me if you know the specifics of other MCUs
    #error unsupported MCU
#endif

    // enable interrupts
    sei();

    // wait for the reference to stabilize
    delay(70);  // see atmel.com/Images/doc8444.pdf on page 9

    // store voltage: ADC = (Vref * 1024) / Vcc <=> Vcc = (Vref * 1024) / ADC
    this->voltage = (float) (((1.1f - this->caliV.offset) * 1024.0f / this->caliV.gain) / (this->accumulate(this->caliV.duration) / float(this->caliV.duration)));

    // restore old mux setting
    ADMUX = mux;

    // check if voltage is positive
    return (0 < this->voltage);
}

float CoreSensors::getTemperature(bool fahrenheit)
{
    return (float) (fahrenheit ? this->temperature * 1.8f + 32.0f : this->temperature);
}

float CoreSensors::getVoltage()
{
    return (float) this->voltage;
}

/**
 * Collects a series (of given length) of measurements and returns the linear sum of all results.
 */
unsigned long CoreSensors::accumulate(unsigned long duration)
{
    long acc = 0;         // accumulator

    this->sample();       // discard first sample (never hurts to be safe)

    // accumulate a series of samples
    for (int i = 0; i < duration; i++) {
        acc += this->sample();
    }

    // return linear sum of sampled values
    return acc;
}

/**
 * Puts the MCU into ADC noise reduction sleep mode (until the measurement is complete) and returns the sampled value.
 */
inline unsigned int CoreSensors::sample()
{
    // save old ADC settings
    byte sra = ADCSRA;                              // store whole ADC settings register A

//    // set it up
//     ADCSRA |= _BV(ADEN) | _BV(ADSC);     // enable ADC, start conversion
//
//     // wait for it
//     while (bit_is_set(ADCSRA, ADSC));    // while conversion is running
//
//     // get the result
//     return ADCL | (ADCH << 8);           // combined conversion value (instead of ADCW)

    // set up ADC register
    ADCSRA |= _BV(ADEN) | _BV(ADIE);                // enable ADC, enable interrupt on finished conversion

    // bring ADC clock between 50kHz and 200kHz (as per AVR documentation)
#if (F_CPU >= 6400000) && (F_CPU <= 25600000)
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // above 6.4MHz: divide it by 128
#elif F_CPU >= 3200000
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1);              // above 3.2MHz:  divide it by 64
#elif F_CPU >= 1600000
    ADCSRA |= _BV(ADPS2) | _BV(ADPS0);              // above 1.6MHz:  divide it by 32
#elif F_CPU >= 800000
    ADCSRA |= _BV(ADPS2);                           // above 0.8MHz:  divide it by 16
#elif F_CPU >= 400000
    ADCSRA |= _BV(ADPS1) | _BV(ADPS0);              // above 0.4MHz:  divide it by 8
#elif F_CPU >= 200000
    ADCSRA |= _BV(ADPS1);                           // above 0.4MHz:  divide it by 4
#elif F_CPU >= 100000
    ADCSRA |= _BV(ADPS0);                           // above 0.2MHz:  divide it by 2
#else
    #error clock speed not supported
#endif

    // set up and enable sleep mode
    sleep_enable();                                 // enable sleep capability via avr sleep library
    set_sleep_mode(SLEEP_MODE_ADC);                 // enable ADC noise reduction sleep mode
    sei();                                          // enable interrupts
    sleep_cpu();                                    // enter low ADC noise sleep mode

    // we're awake again
    ADCSRA = sra;                                   // restore saved ADC settings

    return ADC;                                     // get conversion value directly from register
}

/**
 * We're using ADC noise reduction sleep mode above.
 * Since this capability is based on interrupts, we need an interrupt handler.
 * But since there is no actual functionality to be handled, this handler remains empty.
 */
ISR(ADC_vect)
{
}

CoreSensors CoreSensor;

