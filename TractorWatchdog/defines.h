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

