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
        rpmRotationFlag = false;

        // Disable interrupts whilst reading to avoid race conditions.
        noInterrupts();
        uint32_t rotationTime = rpmCurTime - rpmPrevTime;
        interrupts();

        // Calculate the engine RPM
        state.rpm = 60000000L / rotationTime;

        // A rotation has occured, so the engine must be running.
        if (state.engineState == STOPPED)
        {
            state.engineState = RUNNING;
        }
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

        // Assume this means the engine is stopped.
        if (state.engineState == RUNNING)
        {
            state.engineState = STOPPED;
        }
    }
}

void SensorTime::begin()
{
    EEPROMwl.begin(EEPROM_LAYOUT_VERSION, AMOUNT_OF_INDEXES);
    restoreEEPROM();
}

void SensorTime::tick()
{
    // Did the engine just start or stop?
    if (state.engineState == RUNNING && !isRunning)
    {
        // Engine just started up.
        isRunning = true;
        engineStartTimeTotal = millis();
        engineStartTimeTrip = engineStartTimeTotal;

        // Initial offsets
        totalAtStart = state.totalMinutes;
        tripAtStart = state.tripMinutes;
    }
    else if (state.engineState != RUNNING && isRunning)
    {
        // Engine just stopped.
        isRunning = false;
    }

    // If the engine is running, check if a minute has passed. If so, increment
    // the times.
    if (isRunning)
    {
        bool saveRequired = false;

        // Calculate the new total time and update if needed.
        uint32_t runningTimeTotal = (millis() - engineStartTimeTotal) / 60000;
        uint32_t newTotal = totalAtStart + runningTimeTotal;
        if (newTotal != state.totalMinutes)
        {
            // New time. Write to EEPROM and set state.
            state.totalMinutes = newTotal;
            saveRequired = true;
        }

        // Calculate the new total time and update if needed.
        uint32_t runningTimeTrip = (millis() - engineStartTimeTrip) / 60000;
        uint32_t newTrip = tripAtStart + runningTimeTrip;
        if (newTrip != state.tripMinutes)
        {
            // New time. Write to EEPROM and set state.
            state.tripMinutes = newTrip;
            saveRequired = true;
        }

        // Save to EEPROM if needed.
        if (saveRequired)
        {
            saveEEPROM();
        }
    }
}

void SensorTime::resetTrip()
{
    Serial.println(F("Resetting trip time."));
    state.tripMinutes = 0;
    engineStartTimeTrip = millis();
    saveEEPROM();
}

void SensorTime::restoreEEPROM()
{
    Serial.println(F("Reading from EEPROM"));
    EEPROMwl.get(0, state.totalMinutes);
    EEPROMwl.get(1, state.tripMinutes);
}

void SensorTime::saveEEPROM()
{
    Serial.println(F("Writing to EEPROM"));
    EEPROMwl.put(0, state.totalMinutes);
    EEPROMwl.put(1, state.tripMinutes);
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