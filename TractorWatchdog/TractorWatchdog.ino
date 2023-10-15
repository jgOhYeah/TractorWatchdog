/**
 * @file TractorWatchdog.ino
 * @brief Main code for the Tractor Enginge Watchdog.
 *
 * https://github.com/jgOhYeah/TractorWatchdog
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-02
 */

#include "defines.h"
#include "display.h"
#include "button.h"

// Constructors
#define LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLS 16
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
// LCDGraph<int16_t, LiquidCrystal_I2C> battGraph(6, 0);
// LCDGraph<int16_t, LiquidCrystal_I2C> rpmGraph(8, 0);

State state;

DisplayManager displays(lcd);
Button button(PIN_BUTTON);

void setup()
{
    Serial.begin(SERIAL_BAUD);
    Serial.println(DEVICE_NAME);
    Serial.println(DEVICE_URL);
    Serial.println(COMPILED_MSG);

    // Set up the lcd
    lcd.init();
    lcd.backlight();

    // Setup the button
    button.begin();

    // Show some stuff
    // displays.activate(DISP_ABOUT);
    state.engineState = OIL_PRESSURE;
    state.rpm = 1876;
    state.temperature = 86;
    state.totalHours = 36000;
    state.tripHours = 3600;
    state.voltage = 138;
    displays.updateState(); // Write the status for the max and min.
    displays.activate(DISP_ERROR);
}

void loop()
{
    // Update all the displays.
    displays.tick();
    button.check();
}

/**
 * @brief Function to handle long button presses.
 * 
 * This resets the trip time.
 * 
 */
void btnLongPress()
{
    // TODO.
}

/**
 * @brief Function to handle short button presses.
 * 
 * This changes screens shown.
 * 
 */
void btnShortPress()
{
    displays.next();
}