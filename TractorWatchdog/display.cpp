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
    previous = millis() - interval; // Reset the interval to start now.
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

void DisplayHome::activate()
{
    Display::activate();
    // RPM
    lcd.setCursor(13, 0);
    lcd.print(F("rpm"));

    // Battery voltage
    lcd.setCursor(4, 1);
    lcd.write('V');

    // Temperature
    lcd.setCursor(8, 1);
    lcd.write('C');

    // Trip hours
    lcd.setCursor(15, 1);
    lcd.write('h');
}

void DisplayAbout::intervalTick()
{
    // The white on blue LCDs don't have the best update rate, so do fewer, larger jumps.
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

void DisplayError::activate()
{
    Display::activate();
    // Engine shutdown message.
    lcd.setCursor(0, 0);
    lcd.print(F("ENGINE SHUTDOWN!"));

    // TODO: Error message / explanation
}

void DisplayErrorAlternating::activate()
{
    DisplayIntervalTick::activate();
    error.activate();
    showingHome = false;
}

void DisplayErrorAlternating::deactivate()
{
    DisplayIntervalTick::deactivate();
    if(showingHome)
    {
        home.deactivate();
    }
    else
    {
        error.deactivate();
    }
}

void DisplayErrorAlternating::intervalTick()
{
    if(showingHome)
    {
        home.deactivate();
        error.activate();
    }
    else
    {
        error.deactivate();
        home.activate();
    }
    showingHome = !showingHome;
}

void DisplayManager::tick()
{
    const uint8_t DISPLAY_COUNT = sizeof(displays) / sizeof(Display*);
    for (uint8_t i = 0; i < DISPLAY_COUNT; i++)
    {
        displays[i]->tick();
    }
}

void DisplayManager::next()
{
    // Inform the display it is no longer in control.
    if (currentIndex != DISP_INVALID_INDEX)
    {
        displays[currentIndex]->deactivate();
    }

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

void DisplayManager::activate(DisplayIndex next)
{
    Serial.print(F("Activating display "));
    Serial.print(next);
    Serial.print(F(". Currently on "));
    Serial.println(currentIndex);

    // Deactivate the current display if there is an active one.
    if (currentIndex != DISP_INVALID_INDEX)
    {
        displays[currentIndex]->deactivate();
    }

    // Activate the next
    currentIndex = next;
    displays[currentIndex]->activate();
}

void DisplayManager::updateData(State &state)
{
    const uint8_t DISPLAY_COUNT = sizeof(displays) / sizeof(Display*);
    for (uint8_t i = 0; i < DISPLAY_COUNT; i++)
    {
        displays[i]->updateData(state);
    }
}