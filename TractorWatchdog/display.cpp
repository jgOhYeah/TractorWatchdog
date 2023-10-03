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
    if (active)
    {
        uint32_t current = millis();
        if (current - previous >= interval)
        {
            previous = current;
            intervalTick();
        }
    }
}

void DisplayIntervalTick::activate()
{
    Display::activate();
    previous = millis(); // Reset the interval to start now.
}

void DisplayAbout::activate()
{
    DisplayIntervalTick::activate();
    // Top row
    lcd.setCursor(0, 0);
    lcd.print(DEVICE_NAME);

    // Bottom row
    lcd.setCursor(4, 1);
    lcd.print(DEVICE_URL);
}

void DisplayAbout::intervalTick()
{
    // The white on blue LCDs don't have the best update rate, so do fewer, larger jumps.
    lcd.scrollDisplayLeft();
    lcd.scrollDisplayLeft();
    lcd.scrollDisplayLeft();
    lcd.scrollDisplayLeft();
    lcd.scrollDisplayLeft();
}

void DisplayWaterTemp::activate()
{
    DisplayIntervalTick::activate();
    // Title
    lcd.setCursor(0, 0);
    lcd.print(F("Water Temp"));

    // Current temperature
    lcd.setCursor(15, 0);
    lcd.write('C');

    // LCDGraph
    graph.setRegisters();
    graph.display(0, 1);

    // Max and min temperature
    lcd.setCursor(15, 1);
    lcd.write('C');
}

void DisplayManager::tick()
{
    about.tick();
    temp.tick();
    home.tick();
}

void DisplayManager::activateNext()
{
    // Inform the display it is no longer in control.
    displays[currentIndex]->deactivate();

    // Increment the display. If this is a special display, then it will be
    // set to the home display.
    currentIndex++;
    if (currentIndex >= VIEWABLE_DISPLAYS)
    {
        currentIndex = 0;
    }

    // Activate this display
    displays[currentIndex]->activate();
}