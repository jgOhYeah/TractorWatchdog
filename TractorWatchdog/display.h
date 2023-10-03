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
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Called regularly, even when the display is not currently activated.
     *
     */
    virtual void tick(){};

    /**
     * @brief Signals to the object that it no longer has control of the lcd and
     * should stop drawing stuff on it.
     *
     */
    virtual void deactivate(){};

protected:
    LiquidCrystal_I2C &lcd;
    bool active;
};

/**
 * @brief Adds an interval based tick to the display class. The interval is
 * reset whenever the display is activated and stopped when it is deactivated.
 *
 */
class DisplayIntervalTick : Display
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

protected:
    virtual void intervalTick() {}

private:
    uint32_t previous = 0;
    const uint32_t interval;
};

/**
 * @brief A simple about page that shows more info on the unit.
 *
 */
class DisplayAbout : DisplayIntervalTick
{
    DisplayAbout(LiquidCrystal_I2C &lcd) : DisplayIntervalTick(lcd,);

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Called regularly, even when the display is not currently activated.
     *
     */
    virtual void tick();
};