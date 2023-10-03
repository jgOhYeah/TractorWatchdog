/**
 * @file display.h
 * @brief Header file for the display classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-02
 */
#include "display.h"

void Display::activate()
{
    // Things that should happen for every display.
    lcd.clear();
    active = true;
}

void Display::deactivate()
{
    active = false;
}

void DisplayIntervalTick::tick()
{
    uint32_t current = millis();
    if (current - previous >= interval)
    {
        previous = current;
        intervalTick();
    }
}

void DisplayAbout::activate()
{
    Display::activate();
    // Top row
    lcd.setCursor(0, 0);
    lcd.print(DEVICE_NAME);

    // Bottom row
    lcd.setCursor(4, 1);
    lcd.print(DEVICE_URL);
}

void DisplayAbout::tick()
{
    static uint32_t previous = 0;
}