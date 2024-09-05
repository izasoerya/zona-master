#pragma once

#include <Arduino.h>

struct SensorData
{
    float temperature = 0.0f;
    float humidity = 0.0f;
    float ammonia = 0.0f;
    float lux = 0.0f;
    float windSpeed = 0.0f;
    int led = 0;
    int blower = 0;
    int pump = 0;
    int age = 0;

    String toString()
    {
        return "Temperature: " + String(temperature) + "°C\n" +
               "Humidity: " + String(humidity) + "%\n" +
               "Ammonia: " + String(ammonia) + " ppm\n" +
               "Lux: " + String(lux) + " lux\n" +
               "Wind Speed: " + String(windSpeed) + " m/s\n" +
               "Age:" + String(age) + " hari\n";
    }
};

enum RunMode
{
    NORMAL,
    AUTO
};