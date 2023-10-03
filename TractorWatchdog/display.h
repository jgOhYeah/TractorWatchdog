/**
 * @file display.h
 * @brief Header file for the display classes.
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-02
 */
#pragma once
#include "defines.h"
#include "state.h"

/**
 * @brief Base class for each window that is displayed on the LCD.
 *
 */
class Display
{
public:
    /**
     * @brief Construct a new Display object.
     *
     * @param lcd reference to the LCD to use.
     */
    Display(LiquidCrystal_I2C &lcd) : lcd(lcd){};
    
    /**
     * @brief Called regularly, even when the display is not currently activated.
     *
     */
    virtual void tick(){};

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Signals to the object that it no longer has control of the lcd and
     * should stop drawing stuff on it.
     *
     */
    virtual void deactivate();
    
    /**
     * @brief Called whenever data that the system might have on the screen is
     * updated.
     * 
     * @param state the new data.
     */
    virtual void updateData(State &state) {};

protected:
    LiquidCrystal_I2C &lcd;
    bool active;
};

/**
 * @brief Adds an interval based tick to the display class. The interval is
 * reset whenever the display is activated and stopped when it is deactivated.
 *
 */
class DisplayIntervalTick : public Display
{
public:
    /**
     * @brief Construct a new Display Interval Tick object
     *
     * @param lcd the lcd to write to.
     * @param interval the tick interval in ms.
     */
    DisplayIntervalTick(LiquidCrystal_I2C &lcd, const uint32_t interval) : Display::Display(lcd), interval(interval) {}

    /**
     * @brief Checks if the interval has ellapsed and calls intervalTick if it
     * has.
     *
     */
    virtual void tick();

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

protected:
    /**
     * @brief Method that is called on the interval tick.
     *
     */
    virtual void intervalTick() {}

private:
    uint32_t previous = 0;
    const uint32_t interval;
};

/**
 * @brief A simple about page that shows more info on the unit.
 *
 */
class DisplayAbout : public DisplayIntervalTick
{
public:
    DisplayAbout(LiquidCrystal_I2C &lcd) : DisplayIntervalTick(lcd, 1000) {}

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Called regularly to scroll.
     *
     */
    virtual void intervalTick();
};

/**
 * @brief Class for the main home screen of the display.
 *
 */
class DisplayHome : public Display
{
public:
    using Display::Display;
};

/**
 * @brief Class for the water temperature
 *
 */
class DisplayWaterTemp : public DisplayIntervalTick
{
public:
    DisplayWaterTemp(LiquidCrystal_I2C &lcd) : DisplayIntervalTick(lcd, 2000), graph(8, 0) {}

    /**
     * @brief Starts the graph.
     * 
     */
    void begin()
    {
        graph.begin(&lcd);
        graph.yMin = 0;
        graph.yMax = 110;
        graph.filled = false;
    }

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    LCDGraph<int16_t, LiquidCrystal_I2C> graph;

private:
    bool maxShown = true;
};

/**
 * @brief Display that shows an error message on the screen.
 * 
 */
class DisplayError : public DisplayIntervalTick
{
    using DisplayIntervalTick::DisplayIntervalTick;
};

/**
 * @brief All displays / screens that are being used.
 * 
 */
class DisplayManager
{
public:
    DisplayManager(LiquidCrystal_I2C &lcd) : about(lcd), temp(lcd), home(lcd) {};

    DisplayAbout about;
    DisplayWaterTemp temp;
    DisplayHome home;

    /**
     * @brief Calls the tick function for each display.
     * 
     */
    void tick();

    void activateNext();

private:
    Display* const displays[3] = {&home, &temp, &about};
    const int8_t VIEWABLE_DISPLAYS = 3; // When the display is not one on the viewable list.
    int8_t currentIndex = 0;
};