/**
 * @file display.h
 * @brief Header file for the display classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-02
 */
#include "display.h"

extern State state;

void Display::activate()
{
    // Things that should happen for every display.
    lcd.clear();
    active = true;
    drawState();
}

void Display::deactivate()
{
    active = false;
}

void Display::rightJustify(const int32_t number, const uint8_t digits, const char padding = ' ')
{
    // Convert to a string
    char charBuf[12];
    ltoa(number, charBuf, 10);

    // Print out padding for each column not used.
    int8_t unusedDigits = digits - strlen(charBuf);
    if (unusedDigits < 0)
    {
        // Not enough room to print the number
        Serial.print(F("Number is too long to print"));
        // NOTE: Assumbes that the number is positive and there are at least 2 digits
        lcd.write('>');
        for (uint8_t i = 1; i < digits; i++)
        {
            lcd.write('9');
        }
    }
    else
    {
        while (unusedDigits > 0)
        {
            lcd.write(padding);
            unusedDigits--;
        }

        // Finally print the number.
        lcd.print(charBuf);
    }
}

void Display::drawTenths(const int16_t number, const uint8_t intDigits, const char padding = ' ') 
{
    rightJustify(number / 10, intDigits, padding);
    lcd.write('.');
    lcd.print(number%10);
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

    drawState();
}

void DisplayHome::drawState()
{
    if (active)
    {
        // Engine status
        // The length of each string must be the same so that the remains of the
        // previous isn't left behind.
        lcd.setCursor(0, 0);
        switch (state.engineState)
        {
        case RUNNING:
            lcd.print(F("Running  "));
            break;
        case STOPPED:
            lcd.print(F("Stopped  "));
            break;
        default:
            lcd.print(F("SHUTDOWN "));
        }

        // RPM
        lcd.setCursor(9, 0);
        rightJustify(state.rpm, 4);

        // Battery voltage.
        lcd.setCursor(0, 1);
        drawTenths(state.voltage, 2);

        // Temperature
        lcd.setCursor(5, 1);
        rightJustify(state.temperature, 3);

        // Trip hours
        lcd.setCursor(10, 1);
        uint32_t hourTenths = (state.tripHours / 60) * 10; // Add the hours.
        hourTenths += ((state.tripHours % 60) * 10) / 60; // Add the minutes as tenths of an hour.
        drawTenths(hourTenths, 3);
    }
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

    lcd.setCursor(0, 1);
    switch (state.engineState)
    {
        case STOPPED:
        case RUNNING:
            // Shouldn't be errors for these, but just in case.
            lcd.print(F("Stopped/running?"));
            break;
        case OVER_TEMP:
            lcd.print(F("Over temp " xstr(LIMIT_TEMPERATURE) "C"));
            break;
        case OVER_REV:
            lcd.print(F("Over rev " xstr(LIMIT_REVS) "rpm"));
            break;
        case OIL_PRESSURE:
            lcd.print(F("No oil pressure!"));
            break;
        default:
            // In case an extra error state is added but not entered here.
            lcd.print(F("Indescibable "));
            lcd.print(state.engineState);
    }
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
    if (showingHome)
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
    if (showingHome)
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
    const uint8_t DISPLAY_COUNT = sizeof(displays) / sizeof(Display *);
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

void DisplayManager::drawState()
{
    const uint8_t DISPLAY_COUNT = sizeof(displays) / sizeof(Display *);
    for (uint8_t i = 0; i < DISPLAY_COUNT; i++)
    {
        displays[i]->drawState();
    }
}