/**
 * @file motor.cpp
 * @brief File for the validation and engine shutdown components.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-16
 */
#include "motor.h"

void Motor::begin()
{
    pinMode(PIN_MOTOR_A, OUTPUT);
    pinMode(PIN_MOTOR_B, OUTPUT);
}

void Motor::run()
{
    begin(); // Be safe as we don't want this to fail.
    digitalWrite(PIN_MOTOR_A, HIGH);
    digitalWrite(PIN_MOTOR_B, LOW);
    Serial.println(F("Moving to run position."));
}

void Motor::shutdown()
{
    begin(); // Be safe as we don't want this to fail.
    digitalWrite(PIN_MOTOR_A, LOW);
    digitalWrite(PIN_MOTOR_B, HIGH);
    Serial.println(F("Moving to stop position."));
}