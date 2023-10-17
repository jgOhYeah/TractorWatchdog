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
#include "motor.h"

// Constructors
#define LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLS 16
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

State state;
DisplayManager displays(lcd);
Button button(PIN_BUTTON);
SensorManager sensors;
Motor motor;

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

    // Motor initialisation
    motor.run();

    // Setup the sensors and states
    state.engineState = STOPPED;
    state.rpm = 0;
    state.temperature = 0;
    state.totalMinutes = 0;
    state.tripMinutes = 0;
    sensors.begin();

    // Set up the lcd
    lcd.init();
    lcd.backlight();

    // Setup the button
    button.begin();
    // Show some stuff
    displays.activate(DISP_INIT);
}

void loop()
{
    // Swap from the init display to home screen if needed.
    uint32_t curTime = millis();
    if (curTime > STARTUP_DELAY && displays.currentIndex == DISP_INIT)
    {
        // Still on the init display and time to move on.
        displays.activate(DISP_HOME);
    }

    // Update all the displays.
    displays.tick();
    button.check();
    sensors.tick();

    // Update the sensors every so often.
    static uint32_t prevTime = curTime - SENSOR_UPDATE_INTERVAL - 1; // Run first time
    if (curTime - prevTime > SENSOR_UPDATE_INTERVAL)
    {
        prevTime = curTime;
        // Get new data
        sensors.addState();

        // Check if there are issues
        if (!state.updateEngineState())
        {
            // There were.
            motor.shutdown();
            displays.activate(DISP_ERROR);
        }
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
    lcd.noBacklight();
    sensors.time.resetTrip();
    delay(1000);
    lcd.backlight();
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
    }
}