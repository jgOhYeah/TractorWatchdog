/**
 * @file motor.h
 * @brief File for the validation and engine shutdown components.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-16
 */
#pragma once
#include "defines.h"

class Motor
{
public:
    /**
     * @brief Sets up the output pins.
     *
     */
    void begin();

    /**
     * @brief Moves the solenoid to the run location.
     *
     */
    void run();

    /**
     * @brief Moves the solenoid to the stop position.
     *
     */
    void shutdown();
};