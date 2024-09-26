#pragma once

#include <Arduino.h>
#include "models.h"

class NextionLCD
{
private:
    void endNextionCommand()
    {
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
    }

public:
    NextionLCD();
    void sendHumidityToNextion(SensorData data);
    void sendAgeToNextion(SensorData data);
    void sendTemperatureToNextion(SensorData data);
    void sendAmmoniaToNextion(SensorData data);
    void sendLuxToNextion(SensorData data);
    void sendWindSpeedToNextion(SensorData data);
    void sendDateTimeToNextion(DateTime date);

    void blowerSlider(int value);
    void pumpSlider(int value);
    void ledSlider(int value);

    void printBlower();
    void printPump();
    void printLed();
};