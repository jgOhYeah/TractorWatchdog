/**
 * @file sensors.h
 * @brief File that contains code for sensors
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-15
 */
#include "sensors.h"

extern State state;
extern volatile uint32_t rpmCurTime, rpmPrevTime;
extern volatile bool rpmRotationFlag;
extern void rpmInterrupt();

void SensorBattery::addState()
{
    uint32_t adc = analogRead(PIN_BATTERY);
    state.voltage = adc * CAL_BATT_NUMERATOR / CAL_BATT_DENOMINATOR;
}

void SensorOil::begin()
{
    pinMode(PIN_OIL_SW, INPUT_PULLUP);
}

void SensorOil::addState()
{
    state.oilPressure = digitalRead(PIN_OIL_SW); // TODO: Will be the opposite in the real version.
}

void SensorTemperature::addState()
{
    state.temperature = analogRead(PIN_THERMISTOR_1) / 7; // TODO: Calibration curve.
}

void SensorRPM::begin()
{
    pinMode(PIN_RPM, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_RPM), rpmInterrupt, FALLING);
}

void SensorRPM::tick()
{
    if (rpmRotationFlag)
    {
        // A rotation happened since the last time.
        // Disable interrupts whilst reading to avoid race conditions.
        noInterrupts();
        uint32_t rotationTime = rpmCurTime - rpmPrevTime;
        interrupts();

        state.rpm = 60000000L / rotationTime;

        rpmRotationFlag = false;
    }
}

void SensorRPM::addState()
{
    // Check if there hasn't been a rotation for a while.
    noInterrupts();
    uint32_t elapsed = micros() - rpmCurTime;
    interrupts();
    if (elapsed > 5000000)
    {
        // Assume the RPM is 0.
        state.rpm = 0;
    }
}

void SensorManager::begin()
{
    const uint8_t SENSOR_COUNT = sizeof(sensors) / sizeof(Sensor *);
    for (uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i]->begin();
    }
}

void SensorManager::addState()
{
    const uint8_t SENSOR_COUNT = sizeof(sensors) / sizeof(Sensor *);
    for (uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i]->addState();
    }
}

void SensorManager::tick()
{
    const uint8_t SENSOR_COUNT = sizeof(sensors) / sizeof(Sensor *);
    for (uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i]->tick();
    }
}