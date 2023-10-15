/**
 * @file state.cpp
 * @brief Cpntains code realting to the system state.
 * 
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-10-16
 */
#include "state.h"

bool State::updateEngineState()
{
    bool wasOk = (engineState == RUNNING) || (engineState == STOPPED);

    // TODO: Engine startup delay to build up oil pressure.
    // Order is the order that issues will be shown to the user.
    if (!oilPressure)
    {
        Serial.println(F("No oil pressure"));
        engineState = OIL_PRESSURE;
    }
    else if (temperature > LIMIT_TEMPERATURE)
    {
        Serial.println(F("Over temperature"));
        engineState = OVER_TEMP;
    }
    else if (rpm > LIMIT_REVS)
    {
        Serial.println(F("Over reving"));
        engineState = OVER_REV;
    }
    else
    {
        // Everything ok.
        return true;
    }
    
    // Something failed. Return false if the first time, true otherwise.
    return !wasOk;
}