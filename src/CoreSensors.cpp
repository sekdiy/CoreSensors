/**
 * CoreSensors.cpp
 *
 * An interface to the Arduino's internal temperature (diode reference) and supply voltage (bandgap reference).
 *
 * @author sekdiy (https://github.com/sekdiy/CoreSensors)
 * @date 08.03.2016
 * @version See git comments for changes.
 * @see http://playground.arduino.cc/Main/InternalTemperatureSensor
 *
 * @todo handle proper voltage sensing in MCUs without temperature sensor
 * @todo consider refactoring to Millivolt and Millikelvin in order to get rid of floating point arithmetic
 * @todo find more sensor functionality to include in library
 */

#include <avr/sleep.h>      // built-in AVR sleep library
#include "Arduino.h"        // we're no longer compatible to IDEs < 1.0 (please raise an issue if this is too progressive)
#include "CoreSensors.h"    // https://github.com/sekdiy/CoreSensors

int const CoreSensors::celsius = 0;
int const CoreSensors::fahrenheit = 1;

/**
 * Default calibration profile, works for most devices.
 */
CoreSensorsCalibration const CoreSensors::UncalibratedCoreSensors = { 1.0f, 0.0f, 100, 1.0f, 0.0f, 100 };

/**
 * Applies a custom set of calibration parameters.
 *
 * @param CoreSensorsCalibration The calibration data for the MCU at hand.
 */
void CoreSensors::begin(CoreSensorsCalibration calibration)
{
    this->calibration = calibration;
}

/**
 * Process both temperature and voltage measurements and store results.
 *
 * @return True if either sensor could be processed successfully.
 */
bool CoreSensors::process()
{
    // assume successful processing
    bool result = true;

    // make sure both sensors get processed
    result &= this->processTemperature();
    result &= this->processVoltage();

    // fail if either sensor fails
    return result;
}

/**
 * Process temperature measurement and store result.
 *
 * @return True if temperature sensor could be processed successfully.
 */
bool CoreSensors::processTemperature()
{
    // determine specific ADMUX settings
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega168A__) ||  defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328__) ||  defined(__AVR_ATmega328P__)
    // ATmega 32U4: Arduino Leonardo and compatible
    // ATmega 168A/P:  Arduino Decimilia and older
    // ATmega 328(P): Arduino Duemilanove, Uno and compatible
    #define CoreSensors_Temperature_ADMUX bit(REFS1) | bit(REFS0) |  bit(MUX3)
#else
    // ATmega8: No
    // ATmega8L: No
    // ATmega8A: No
    // ATmega168: No
    // ATmega1280 (Arduino Mega): No
    // ATmega2560 (Arduino Mega 2560): No
    // please raise an issue if you know the specifics of other MCUs
    return false;
#endif

    // save old mux setting
    byte mux = ADMUX;

    // select internal temperature sensor
    cli();                                  // disable interrupts
    ADMUX = CoreSensors_Temperature_ADMUX;  // apply
    sei();                                  // enable interrupts

    // wait for the reference to stabilize
    delay(20);                              // TODO determine optimal value

    // store temperature
    this->temperature = (float) ((this->accumulate(this->calibration.lengthT) / float(this->calibration.lengthT) - 273.15f - this->calibration.offsetT) / this->calibration.gainT);

    // restore old mux setting
    cli();                                  // disable interrupts
    ADMUX = mux;                            // restore
    sei();                                  // enable interrupts

    // check if temperature is plausible (datasheet specifies -40degC – 85degC after calibration)
    return (-40 < this->temperature) && (85 > this->temperature);
}

/**
 * Process voltage measurement and store result.
 *
 * @return True if voltage sensor could be processed successfully.
 */

bool CoreSensors::processVoltage()
{
    // determine specific ADMUX settings
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    // ATmega 32U4: Arduino Leonardo and compatible
    // ATmega 1280: Arduino Mega and compatible
    // ATmega 2560: Arduino Mega 2560 and compatible
    #define CoreSensors_Voltage_ADMUX _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)
#elif defined(__AVR_ATmega168__) ||  defined(__AVR_ATmega328__) || defined(__AVR_ATmega168P__) ||  defined(__AVR_ATmega328P__)
    // ATmega 168:  Arduino Decimilia and older
    // ATmega 328P: Arduino Duemilanove, Uno and compatible
    #define CoreSensors_Voltage_ADMUX _BV(REFS0) | _BV(MUX3) | _BV(MUX2) |_BV (MUX1);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    // ATTinyX5: various types
    #define CoreSensors_Voltage_ADMUX _BV(MUX3) | _BV(MUX2);
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    // ATTinyX4: various types
    #define CoreSensors_Voltage_ADMUX _BV(MUX5) | _BV(MUX0);
#else
    // please raise an issue if you know the specifics of other MCUs
    return false;
#endif

    // save old mux setting
    byte mux = ADMUX;

    // select internal band gap reference
    cli();                                  // disable interrupts
    ADMUX = CoreSensors_Voltage_ADMUX;      // apply
    sei();                                  // enable interrupts

    // wait for the reference to stabilize
    delay(70);  // see atmel.com/Images/doc8444.pdf on page 9

    // store voltage: ADC = (Vref * 1024) / Vcc <=> Vcc = (Vref * 1024) / ADC
    this->voltage = (float) (((1.1f - this->calibration.offsetV) * 1024.0f / this->calibration.gainV) / (this->accumulate(this->calibration.lengthV) / float(this->calibration.lengthV)));

    // restore old mux setting
    cli();                                  // disable interrupts
    ADMUX = mux;                            // restore
    sei();                                  // enable interrupts

    // check if voltage is plausible (datasheet specifies 1.8 – 5.5V)
    return (1.8 < this->voltage) && (5.5 > this->voltage);
}

/*
 * @param bool True for degrees Fahrenheit, Celsius otherwise.
 * @return The temperature (in degrees Celsius or Fahrenheit).
 */
float CoreSensors::getTemperature(bool fahrenheit)
{
    return (float) (fahrenheit ? this->temperature * 1.8f + 32.0f : this->temperature);
}

/*
 * @return The voltage (in Volt).
 */
float CoreSensors::getVoltage()
{
    return (float) this->voltage;
}

/**
 * Collects a series (of given length) of measurements and returns the linear sum of all results.
 *
 * @param The number of samples to be aquired.
 * @return The linear sum of all sampled values.
 */
unsigned long CoreSensors::accumulate(unsigned long length)
{
    unsigned long acc = 0;  // accumulator

    this->sample();         // discard first sample (never hurts to be safe)

    // accumulate a series of samples
    for (unsigned long i = 0; i < length; i++) {
        acc += this->sample();
    }

    // return linear sum of sampled values
    return acc;
}

/**
 * Puts the MCU into ADC noise reduction sleep mode (until the measurement is complete) and returns the sampled value.
 *
 * @return The single sample value.
 */
inline unsigned int CoreSensors::sample()
{
    // disable interrupts
    cli();

    // save old ADC settings
    byte sra = ADCSRA;                              // store whole ADC settings register A

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
    // please raise an issue if you find unsupported configurations
    #error clock speed not supported
#endif

    // set up and enable sleep mode
    sleep_enable();                                 // enable sleep capability via avr sleep library
    set_sleep_mode(SLEEP_MODE_ADC);                 // enable ADC noise reduction sleep mode
    sei();                                          // enable interrupts (very important)
    sleep_cpu();                                    // enter low ADC noise sleep mode

    // we're awake again
    cli();                                          // disable interrupts
    unsigned int result = ADC;                      // get conversion value directly from register
    ADCSRA = sra;                                   // restore saved ADC settings
    sei();                                          // enable interrupts

    return result;
}

/**
 * We're using ADC noise reduction sleep mode above.
 * Since this capability is based on interrupts, we need an interrupt handler (in order to ever wake up again).
 * But since there is no actual functionality to be handled, this handler remains empty.
 *
 * This ADC_vect can be replaced by a different one without breaking the CoreSensors library.
 */
ISR(ADC_vect)
{
}

/**
 * The CoreSensor object, a singleton that gives access to the core sensors (there is only one AVR core!).
 */
CoreSensors CoreSensor;

