/**
 * @file display.cpp
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
        // NOTE: Assumes that the number is positive and there are at least 2 digits
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
    lcd.print(number % 10);
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

    // Dodgy offset the display so that the first scroll puts it in the correct
    // starting position. // TODO: Fix
    lcd.scrollDisplayRight();
    lcd.scrollDisplayRight();
    lcd.scrollDisplayRight();
    lcd.scrollDisplayRight();
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

void DisplayHome::drawState()
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
    hourTenths += ((state.tripHours % 60) * 10) / 60;  // Add the minutes as tenths of an hour.
    drawTenths(hourTenths, 3);
}

void DisplayAbout::intervalTick()
{
    // The white on blue LCDs don't have the best update rate, so do fewer, larger jumps.
    lcd.scrollDisplayLeft();
    lcd.scrollDisplayLeft();
    lcd.scrollDisplayLeft();
    lcd.scrollDisplayLeft();
}

Graph::Graph(LiquidCrystal_I2C &lcd, uint8_t graphWidth) : lcd(lcd), graph(graphWidth, 0)
{
    // Mose well call begin in the constructor as only setting variables.
    graph.begin(&lcd);
    graph.filled = false;
}

void Graph::addData(int16_t data)
{
    lastPointAccumulator += data;
    lastPoints++;

    // Set max and mins
    if (graph.length())
    {
        // There is data in the graph.
        if (data < graph.yMin)
        {
            graph.yMin = data;
        }
        else
        {
            graph.yMax = data;
        }
    }
    else
    {
        // No data in the graph. This is the new max and min.
        graph.yMax = data;
        graph.yMin = data;
    }

    // If we have enough points, plot.
    if (lastPoints >= GRAPH_PLOT_EVERY)
    {
        addAveragePoint();
    }
}

void Graph::addAveragePoint()
{
    graph.add(lastPointAccumulator / lastPoints);
    lastPointAccumulator = 0;
    lastPoints = 0;
}

void Graph::setRegisters()
{
    graph.setRegisters();
}

void Graph::display()
{
    graph.display(0, 1);
}

void DisplayWaterTemp::activate()
{
    Display::activate();
    // Title
    lcd.setCursor(0, 0);
    lcd.print(F("Water Temp"));

    // Current temperature
    lcd.setCursor(15, 0);
    lcd.write('C');

    // LCDGraph
    graph.setRegisters();
    graph.display();

    // Max temperature
    lcd.setCursor(9, 1);
    lcd.print(F("Max"));
    lcd.setCursor(15, 1);
    lcd.write('C');
}

void DisplayWaterTemp::updateData()
{
    graph.addData(state.temperature);
}

void DisplayWaterTemp::drawState()
{
    // Current temperature
    lcd.setCursor(12, 0);
    rightJustify(state.temperature, 3);

    // Graph (display has already been called).
    graph.setRegisters();

    // Max temperature
    lcd.setCursor(12, 1);
    rightJustify(graph.graph.yMax, 3);
}

void DisplayVoltage::activate()
{
    DisplayIntervalTick::activate();
    // Title
    lcd.setCursor(0, 0);
    lcd.print(F("Battery"));

    // Current voltage
    lcd.setCursor(15, 0);
    lcd.write('V');

    // LCDGraph
    graph.setRegisters();
    graph.display();

    // Max and min voltage
    lcd.setCursor(15, 1);
    lcd.write('V');
}

void DisplayVoltage::updateData()
{
    graph.addData(state.voltage);
}

void DisplayVoltage::drawState()
{
    // Current temperature
    lcd.setCursor(11, 0);
    drawTenths(state.voltage, 2);

    // Graph (display has already been called).
    graph.setRegisters();

    // Max and min temperature
    lcd.setCursor(7, 1);
    if (maxShown)
    {
        // Draw the maximum temperature.
        lcd.print(F("Max "));
        drawTenths(graph.graph.yMax, 2);
    }
    else
    {
        // Draw the minimum temperature.
        lcd.print(F("Min "));
        drawTenths(graph.graph.yMin, 2);
    }
}

void DisplayVoltage::intervalTick()
{
    maxShown = !maxShown;
    drawState();
}

void DisplayError::activate()
{
    Display::activate();
    // Engine shutdown message.
    lcd.setCursor(0, 0);
    lcd.print(F("ENGINE SHUTDOWN!"));
}

void DisplayError::drawState()
{
    lcd.setCursor(0, 1);
    switch (state.engineState)
    {
    case STOPPED:
    case RUNNING:
        // Shouldn't be errors for these, but just in case.
        lcd.print(F("I'm confused :)"));
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
        lcd.print(F("Something else"));
        lcd.print(state.engineState);
    }

    // Write some spaces to wipe out any remaining text from a previous error.
    for (uint8_t i = 0; i < 16; i++)
    {
        lcd.write(' ');
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

void DisplayManager::updateState()
{
    // Call updateData for each data point.
    const uint8_t DISPLAY_COUNT = sizeof(displays) / sizeof(Display *);
    for (uint8_t i = 0; i < DISPLAY_COUNT; i++)
    {
        displays[i]->updateData();
    }

    // For the active display, call its function to draw the new data.
    if (currentIndex != DISP_INVALID_INDEX)
    {
        displays[currentIndex]->drawState();
    }
}