#pragma once

#include <Arduino.h>

struct SensorData
{
    float temperature = 0.0f;
    float humidity = 0.0f;
    float ammonia = 0.0f;
    int lux = 0;
    int windSpeed = 0;
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

struct DateTime
{
    int year = 2024;
    int month = 9;
    int day = 26;
};

enum RunMode
{
    NORMAL,
    AUTO
};