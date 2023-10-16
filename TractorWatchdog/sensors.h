/**
 * @file sensors.h
 * @brief File that contains code for sensors
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-15
 */
#pragma once
#include "defines.h"
#include "state.h"

/**
 * @brief Base class for all sensors
 *
 */
class Sensor
{
public:
    /**
     * @brief Called to initialise during runtime if needed.
     *
     */
    virtual void begin() {}

    /**
     * @brief Takes a reading and updates the global state variable.
     *
     */
    virtual void addState() {}

    /**
     * @brief Called regularly.
     *
     */
    virtual void tick() {}
};

/**
 * @brief Class for measuring the battery voltage
 *
 */
class SensorBattery : public Sensor
{
public:
    /**
     * @brief Measures the battery voltage.
     *
     */
    virtual void addState();
};

/**
 * @brief Measures whether the oil pressure switch is tripped.
 *
 */
class SensorOil : public Sensor
{
public:
    /**
     * @brief Sets up the input pins.
     *
     */
    virtual void begin();

    /**
     * @brief Checks if the oil pressure switch is tripped.
     *
     */
    virtual void addState();
};

/**
 * @brief Class for measuring the temperature
 *
 */
class SensorTemperature : public Sensor
{
public:
    /**
     * @brief Measures the temperature.
     *
     */
    virtual void addState();
};

/**
 * @brief Class for logging RPM
 *
 */
class SensorRPM : public Sensor
{
public:
    /**
     * @brief Initialises the interrupts and pins.
     *
     */
    virtual void begin();

    /**
     * @brief Because this is interrupt based, updates on a tick instead of
     * addState.
     *
     */
    virtual void tick();

    /**
     * @brief Checks if there hasn't been an interrupt recently.
     *
     * Resets the rpm to 0 if so.
     */
    virtual void addState();
};

class SensorTime : public Sensor
{
public:
    /**
     * @brief Initialises the EEPROM and loads any existing times from it.
     * 
     */
    virtual void begin();

    /**
     * @brief Updates the time as needed.
     * 
     */
    virtual void tick();

    /**
     * @brief Resets the trip time
     * 
     */
    virtual void resetTrip();

private:
    /**
     * @brief Saves the times stored in the state variable to EEPROM.
     * 
     */
    void saveEEPROM();

    /**
     * @brief Restores the times stored in the state variable from EEPROM.
     * 
     */
    void restoreEEPROM();

    bool isRunning = false;
    uint32_t engineStartTimeTotal = 0;
    uint32_t engineStartTimeTrip = 0; // Separate so this can be reset independently.
    uint32_t totalAtStart, tripAtStart;
};

/**
 * @brief Class for managing sensors.
 *
 */
class SensorManager
{
public:
    /**
     * @brief Calls begin for each sensor.
     *
     */
    void begin();

    /**
     * @brief Calls each sensor to take a reading.
     *
     */
    void addState();

    /**
     * @brief Calls tick for each sensor.
     *
     */
    void tick();

    SensorBattery battery;
    SensorOil oil;
    SensorTemperature temperature;
    SensorRPM rpm;
    SensorTime time;

    Sensor *const sensors[5] = {&battery, &oil, &temperature, &rpm, &time};
};