/**
 * @file state.h
 * @brief A class for the current system state.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-03
 */

#pragma once

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
struct State
{
    int16_t temperature;
    uint8_t voltage;
    uint32_t tripHours;
    uint32_t totalHours;
    uint16_t rpm;
    EngineState engineState;
};