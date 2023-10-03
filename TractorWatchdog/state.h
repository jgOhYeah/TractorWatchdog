/**
 * @file state.h
 * @brief A class for the current system state.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-03
 */

#pragma once

enum EngineState
{
    RUNNING = 0,
    STOPPED = 1,
    ERROR = 2
};

struct State
{
    int16_t temperature;
    uint8_t voltage;
    uint32_t tripHours;
    uint32_t totalHours;
    uint16_t engineSpeed;
    EngineState engineState;
};