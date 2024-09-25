#include "nextion.h"

NextionLCD::NextionLCD() {}

void NextionLCD::sendAgeToNextion(SensorData data)
{
    String command = "umur.txt=\"" + String(data.age) + "\"";
    Serial2.print(command);
    endNextionCommand();
}

void NextionLCD::sendHumidityToNextion(SensorData data)
{
    String command = "humidity.txt=\"" + String(data.humidity) + "\"";
    Serial2.print(command);
    endNextionCommand();
}

void NextionLCD::sendTemperatureToNextion(SensorData data)
{
    String command = "suhu.txt=\"" + String(data.temperature, 1) + "\"";
    Serial2.print(command);
    endNextionCommand();
}

void NextionLCD::sendAmmoniaToNextion(SensorData data)
{
    String command = "amonia.txt=\"" + String(data.ammonia, 1) + "\"";
    Serial2.print(command);
    endNextionCommand();
}

void NextionLCD::sendLuxToNextion(SensorData data)
{
    String command = "lux.txt=\"" + String(data.lux) + "\"";
    Serial2.print(command);
    endNextionCommand();
}

void NextionLCD::sendWindSpeedToNextion(SensorData data)
{
    String command = "wind.txt=\"" + String(data.windSpeed) + "\"";
    Serial2.print(command);
    endNextionCommand();
}

void NextionLCD::sendDateTimeToNextion(DateTime date)
{
    String commandDay = "day.txt=\"" + String(date.day) + "\"";
    Serial2.print(commandDay);
    endNextionCommand();
    String commandMonth = "month.txt=\"" + String(date.month) + "\"";
    Serial2.print(commandMonth);
    endNextionCommand();
    String commandYear = "year.txt=\"" + String(date.year) + "\"";
    Serial2.print(commandYear);
    endNextionCommand();
}

void NextionLCD::blowerSlider(int value)
{
    String command = "h0.val=" + String(value);
    Serial2.print(command);
    endNextionCommand();
}

void NextionLCD::pumpSlider(int value)
{
    String command = "h1.val=" + String(value);
    Serial2.print(command);
    endNextionCommand();
}

void NextionLCD::ledSlider(int value)
{
    String command = "h2.val=" + String(value);
    Serial2.print(command);
    endNextionCommand();
}
