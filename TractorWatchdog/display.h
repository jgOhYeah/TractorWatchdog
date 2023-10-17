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
     * updated. Also called upon activating the state.
     *
     * @param state the new data.
     */
    virtual void drawState(){};

    /**
     * @brief Called whenever there is a new state available.
     *
     */
    virtual void updateData(){};

    bool active;

protected:
    /**
     * @brief Right justifies and prints a number to the display.
     *
     * @param number the number to print.
     * @param digits the number of digits.
     * @param padding the character to pad with.
     */
    void rightJustify(const int32_t number, const uint8_t digits, const char padding = ' ');

    /**
     * @brief Prints a number in tenths.
     *
     * Note the issues with leaving out zeros in the tenths position if you
     * attempt to use this for hundreds.
     *
     * @param number the number to print in tenths.
     * @param intDigits the number of digits on the integer side.
     * @param padding the character to pad with.
     */
    void drawTenths(const int16_t number, const uint8_t intDigits, const char padding = ' ');

    LiquidCrystal_I2C &lcd;
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

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Called whenever data that the system might have on the screen is
     * updated.
     *
     */
    virtual void drawState();
};

/**
 * @brief Base class for displays with a graph.
 *
 */
class Graph
{
public:
    Graph(LiquidCrystal_I2C &lcd, uint8_t graphWidth);

    /**
     * @brief Adds a datapoint to be averaged. The max and min is also set using
     * these.
     *
     * If there are more than GRAPH_PLOT_EVERY points, then add to the graph.
     *
     * @param data the point to add.
     */
    void addData(int16_t data);

    /**
     * @brief Sets the registers and draws the graph on the screen.
     *
     */
    void setRegisters();

    /**
     * @brief Draws the graph in the bottom left corner.
     *
     * @code setRegisters() @endcode needs to be called separetely.
     *
     */
    void display();

    LCDGraph<int16_t, LiquidCrystal_I2C> graph;

protected:
    /**
     * @brief Averages the points since this was last called and adds this to
     * the graph.
     *
     */
    void addAveragePoint();

    LiquidCrystal_I2C &lcd;
    int32_t lastPointAccumulator = 0;
    uint16_t lastPoints = 0;
};

/**
 * @brief Class for the water temperature
 *
 */
class DisplayWaterTemp : public Display
{
public:
    DisplayWaterTemp(LiquidCrystal_I2C &lcd) : Display(lcd), graph(lcd, 8) {}

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Called whenever there is a new state available.
     *
     * Updates the graph, max and min.
     *
     */
    virtual void updateData();

    /**
     * @brief Called whenever data that the system might have on the screen is
     * updated.
     *
     */
    virtual void drawState();

private:
    bool maxShown = true;
    Graph graph;
};

/**
 * @brief Class for the water temperature
 *
 */
class DisplayVoltage : public DisplayIntervalTick
{
public:
    DisplayVoltage(LiquidCrystal_I2C &lcd) : DisplayIntervalTick(lcd, 4000), graph(lcd, 6) {}

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Called whenever there is a new state available.
     *
     * Updates the graph, max and min.
     *
     */
    virtual void updateData();

    /**
     * @brief Called whenever data that the system might have on the screen is
     * updated.
     *
     */
    virtual void drawState();

    /**
     * @brief Method that is called on the interval tick.
     *
     * Swaps between max and min voltage
     *
     */
    virtual void intervalTick();

private:
    bool maxShown = true;
    Graph graph;
};

/**
 * @brief Display that shows an error message on the screen.
 *
 */
class DisplayError : public Display
{
public:
    using Display::Display;

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Called whenever data that the system might have on the screen is
     * updated.
     *
     */
    virtual void drawState();
};

/**
 * @brief Alternates between display error and home regularly.
 *
 */
class DisplayErrorAlternating : public DisplayIntervalTick
{
public:
    DisplayErrorAlternating(
        LiquidCrystal_I2C &lcd, DisplayError &dispErr, DisplayHome &dispHome)
        : DisplayIntervalTick(lcd, 2000), error(dispErr), home(dispHome){};

    /**
     * @brief Activates the error display first.
     *
     */
    virtual void activate();

    /**
     * @brief Deactivates the current display (home or error)
     *
     */
    virtual void deactivate();

    /**
     * @brief Calls drawState for the currently active display.
     * 
     */
    virtual void drawState();

protected:
    /**
     * @brief Swaps beteen home and error.
     *
     */
    virtual void intervalTick();

private:
    DisplayError error;
    DisplayHome home;
    bool showingHome;
};

/**
 * @brief Display that shows the total and trip times.
 *
 */
class DisplayTime : public Display
{
public:
    using Display::Display;

    /**
     * @brief Draws the display as the current one on the screen.
     *
     */
    virtual void activate();

    /**
     * @brief Called whenever data that the system might have on the screen is
     * updated.
     *
     */
    virtual void drawState();
};

/**
 * @brief Convenient indices / names corresponding to the array of displays.
 * 
 */
enum DisplayIndex
{
    DISP_HOME,
    DISP_TEMPERATURE,
    DISP_VOLTAGE,
    DISP_TIME,
    DISP_ABOUT,
    DISP_ERROR_SINGLE,
    DISP_ERROR,
    DISP_INIT,
    DISP_INVALID_INDEX = 255
};

/**
 * @brief All displays / screens that are being used.
 *
 */
class DisplayManager
{
public:
    DisplayManager(LiquidCrystal_I2C &lcd)
        : about(lcd), temp(lcd), voltage(lcd), home(lcd), time(lcd),
          errorSingle(lcd), error(lcd, errorSingle, home){};

    /**
     * @brief Calls the tick function for each display.
     *
     */
    void tick();

    /**
     * @brief Activates the next display that the user can cycle through using
     * the button. If a different screen (like the error screen) is shown, then
     * the home display is activated.
     *
     */
    void next();

    /**
     * @brief Deactivates the current and activates the given display.
     *
     * @param next the index of the display to activate.
     */
    void activate(DisplayIndex next);

    /**
     * @brief Called to update data from the current state.
     *
     * @param state the new data.
     */
    virtual void updateState();

    /**
     * @brief Don't set this directly. Instead use activate() or next().
     * 
     */
    uint8_t currentIndex = DISP_INVALID_INDEX;

private:
    DisplayAbout about;
    DisplayWaterTemp temp;
    DisplayHome home;
    DisplayVoltage voltage;
    DisplayTime time;
    DisplayError errorSingle;
    DisplayErrorAlternating error;

    Display *const displays[8] = {&home, &temp, &voltage, &time, &about, &errorSingle, &error, &time};
    const int8_t VIEWABLE_DISPLAYS = 4; // When the display is not one on the viewable list.
};