/**
 * @file button.cpp
 * @brief File that manages input from the button.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-02
 */
#include "button.h"

extern void btnLongPress();
extern void btnShortPress();

void Button::begin()
{
    pinMode(pin, INPUT_PULLUP);
}

void Button::check()
{
    bool curPressed = digitalRead(pin);
    uint32_t curTime = millis();

    if (curPressed)
    {
        // The button is pressed this round. Reset the debounce timer.
        lastPressedTime = curTime;

        // Check if this is the first time the button was pressed and set the
        // start time if so.
        if (!isPressed)
        {
            isPressed = true;
            pressStartTime = curTime;
        }
    }
    else if (isPressed && (curTime - lastPressedTime) > UI_DEBOUNCE_TIME)
    {
        // Not pressed but was last time we checked. The debounce timer has also
        // elapsed - we are allowed to say the button is no longer pressed.
        isPressed = false;

        // Long or short press?
        if (curTime - pressStartTime > UI_LONG_PRESS_TIME)
        {
            Serial.println(F("Long press"));
            btnLongPress();
        }
        else
        {
            Serial.println(F("Short press"));
            btnShortPress();
        }
    }
}