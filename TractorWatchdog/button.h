/**
 * @file button.h
 * @brief File that manages input from the button.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-15
 */
#pragma once
#include "defines.h"

/**
 * @brief Class for handling input from a user facing button.
 * 
 * This class calls a function for a long and short press.
 * 
 */
class Button
{
public:
    /**
     * @brief Construct a new Button object
     *
     * @param pin the Arduino pin to use
     */
    Button(const uint8_t pin) : pin(pin) {}

    /**
     * @brief Sets up the pin as INPUT_PULLUP
     *
     */
    void begin();

    /**
     * @brief Checks if the button is pressed / released and calls the required functions.
     *
     */
    void check();

private:
    const uint8_t pin;
    bool isPressed = false;
    bool longEnabled = true;
    uint32_t lastPressedTime;
    uint32_t pressStartTime;
};