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

// Constructors
#define LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLS 16
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
LCDGraph<int16_t, LiquidCrystal_I2C> tempGraph(8, 0);
LCDGraph<int16_t, LiquidCrystal_I2C> battGraph(6, 0);
LCDGraph<int16_t, LiquidCrystal_I2C> rpmGraph(8, 0);

void setup()
{
    Serial.begin(SERIAL_BAUD);
    Serial.println(DEVICE_NAME);
    Serial.println(DEVICE_URL);
    Serial.println(COMPILED_MSG);
}

void loop()
{
}