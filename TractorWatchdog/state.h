/**
 * @file state.h
 * @brief A class for the current system state.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-03
 */

#pragma once
#include "display.h"

/**
 * @brief The current engine state.
 *
 */
enum EngineState
{
    RUNNING,
    STOPPED,
    OVER_TEMP,
    OVER_REV,
    OIL_PRESSURE
};

/**
 * @brief Struct to hold the current state and parameters of the vehicle.
 *
 */
class State
{
public:
    int16_t temperature;
    uint8_t voltage;
    uint32_t tripHours;
    uint32_t totalHours;
    uint16_t rpm;
    bool oilPressure; // True if there is pressure.
    EngineState engineState;

    /**
     * @brief Updates the engineState attribute from the other state attributes.
     * 
     * @return true if everything is ok.
     * @return false if there is an issue (only returned on the first issue, all
     *               issues need to be cleared before this can go true again).
     */
    bool updateEngineState();
};