/**
 * @file defines.h
 * @brief Pin and other definitions
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-02
 */
#pragma once
#define VERSION "0.1"

// Libraries
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LCDGraph.h>

// Constants
#define SERIAL_BAUD 38400

#define DEVICE_NAME F("Tractor Watchdog - Jotham Gates - 2023")
#define DEVICE_URL F("github.com/jgOhYeah/TractorWatchdog")
#define COMPILED_MSG F("Version " VERSION ". Compiled " __DATE__)

#define GRAPH_PLOT_EVERY 9 // How many data points to average for each graph point.
// 9 data points at 40 wide should be 20 minutes across the x axis.
#define UI_DEBOUNCE_TIME 50
#define UI_LONG_PRESS_TIME 5000

// Limits
#define LIMIT_TEMPERATURE 110
#define LIMIT_REVS 1900

// Pins
// H Bridge wires
#define PIN_MOTOR_A 3
#define PIN_MOTOR_B 5
#define PIN_HORN 6
#define PIN_AUX 11

// RS485
#define PIN_RS485_DI 1
#define PIN_RS485_RO 0
#define PIN_RS485_NRE 4
#define PIN_RS485_DE 7

// Sensors
#define PIN_RPM 2
#define PIN_OIL_SW 8
#define PIN_THERMISTOR_1 A0
#define PIN_THERMISTOR_2 A1
#define PIN_BATTERY A2

// UI
#define PIN_BUTTON A3
#define PIN_SDA A4
#define PIN_SCL A5

// Macros to stringify
#define xstr(s) str(s)
#define str(s) #s