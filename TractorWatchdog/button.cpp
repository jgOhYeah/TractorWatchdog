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
    bool curPressed = !digitalRead(pin);
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

        // Check if a long press and call the function now instead of on
        // release.
        if (curTime - pressStartTime > UI_LONG_PRESS_TIME && longEnabled)
        {
            // Long press
            Serial.println(F("Long press"));
            btnLongPress();

            // Make sure we only send long press once
            longEnabled = false;
        }

    }
    else if (isPressed && (curTime - lastPressedTime) > UI_DEBOUNCE_TIME)
    {
        // Not pressed but was last time we checked. The debounce timer has also
        // elapsed - we are allowed to say the button is no longer pressed.
        isPressed = false;
        longEnabled = true;

        // Long or short press? Only call for a short press as long is called on
        // the threshold being reached above.
        if (curTime - pressStartTime <= UI_LONG_PRESS_TIME)
        {
            // Short press.
            Serial.println(F("Short press"));
            btnShortPress();
        }
    }
}