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
#include "sensors.h"

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
SensorManager sensors;

// Variables for rpm measurement
volatile uint32_t rpmCurTime = 0;
volatile uint32_t rpmPrevTime = 0;
volatile bool rpmRotationFlag = false;

void setup()
{
    Serial.begin(SERIAL_BAUD);
    Serial.println(DEVICE_NAME);
    Serial.println(DEVICE_URL);
    Serial.println(COMPILED_MSG);

    // Setup the sensors and states
    state.engineState = STOPPED;
    state.rpm = 0;
    state.temperature = 0;
    state.totalHours = 0;
    state.tripHours = 0;
    sensors.begin();

    // Set up the lcd
    lcd.init();
    lcd.backlight();

    // Setup the button
    button.begin();
    // Show some stuff
    displays.updateState(); // Write the status for the max and min.
    displays.activate(DISP_ERROR);
}

void loop()
{
    // Update all the displays.
    displays.tick();
    button.check();
    sensors.tick();

    // Update the sensors every so often.
    static uint32_t prevTime = 0;
    uint32_t curTime = millis();
    if (curTime - prevTime > SENSOR_UPDATE_INTERVAL)
    {
        prevTime = curTime;
        sensors.addState();
        displays.updateState();
    }
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

/**
 * @brief ISR for RPM interrupts.
 *
 * Relates to code in sensors.h and sensors.cpp.
 *
 */
void rpmInterrupt()
{
    uint32_t now = micros();
    if (now - rpmCurTime >= RPM_DEBOUNCE_TIME)
    {
        // Switch closed and no event interrupts for the last little while.
        // Assume this is a genuine rotation and not switch bounce.
        rpmPrevTime = rpmCurTime;
        rpmCurTime = now;
        rpmRotationFlag = true;
        Serial.println("ISR");
    }
}