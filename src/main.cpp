#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <regex> // Include the regex library
#include <SoftwareSerial.h>

#include "time.h"
#include "models.h"
#include "nextion.h"

// WiFi credentials
const char *ssid = "Subhanallah";
const char *password = "muhammadnabiyullah";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200; // Replace with your GMT offset (seconds)
const int daylightOffset_sec = 0; // Replace with your daylight offset (seconds)

// MQTT broker details
const char *mqtt_server = "zona-api.theunra.site";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
SensorData data;
NextionLCD lcd;
JsonDocument doc;
DateTime date;
EspSoftwareSerial::UART myPort;

RunMode mode = NORMAL;

void setup_wifi();
void reconnect();
bool readSensor();
void printLocalTime();
void assignCommand(char response[]);
void callback(char *topic, byte *message, unsigned int length);
void autoMode();

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  myPort.begin(9600, SWSERIAL_8N1, 12, 13, false);

  randomSeed(analogRead(0));

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  date.day = timeinfo.tm_mday;
  date.month = timeinfo.tm_mon + 1;
  date.year = timeinfo.tm_year + 1900;
  Serial.println(String(date.day) + "/" + String(date.month) + "/" + String(date.year));
}

// Function to find the pattern using regex and capture the subsequent data
String findPattern(const String &str)
{
  std::regex pattern("(00|01|02|03)(.*)");
  std::smatch match;
  std::string s = str.c_str();

  if (std::regex_search(s, match, pattern))
  {
    return String(match.str(1).c_str()) + String(match.str(2).c_str());
  }
  return "";
}

char response[100];

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  bool responseSensor = readSensor();
  if (responseSensor)
  {
    lcd.sendAgeToNextion(data);
    lcd.sendHumidityToNextion(data);
    lcd.sendTemperatureToNextion(data);
    lcd.sendAmmoniaToNextion(data);
    lcd.sendLuxToNextion(data);
    lcd.sendWindSpeedToNextion(data);
    lcd.sendDateTimeToNextion(date);
    // printLocalTime();

    delay(100);
  }

  memset(response, 0, sizeof(response));

  while (Serial2.available())
  {
    String responseString = Serial2.readString();
    responseString.trim();
    Serial.println("data: " + responseString);

    // Modify the findPattern logic to handle null or char '0'
    String result = findPattern(responseString);

    if (result.length() > 0)
    {
      result.trim();

      // Copy the result into the response buffer and ensure it's null-terminated
      strncpy(response, result.c_str(), sizeof(response) - 1);
      response[sizeof(response) - 1] = '\0'; // Ensure null-termination

      // Loop through the string and handle null or '0' character cases
      for (int i = 0; i < result.length(); i++)
      {
        char c = result.charAt(i);

        // Check if it's a null character
        if (c == '\0')
        {
          Serial.println("Null character detected");
          continue; // Skip processing null character
        }
        else
        {
          // Process normally if it's a valid character
          Serial.print("Processing char: ");
          Serial.println(c);

          // Convert the char to its decimal ASCII value and process as needed
          int value = (int)c; // Convert char to decimal ASCII value
          Serial.print("Decimal value: ");
          Serial.println(value);

          // You can store or use the decimal value without modifying the null terminator
        }
      }

      break; // Exit the loop after finding the pattern
    }
  }

  if (response[0] != 0)
  {
    Serial.println(response);
    assignCommand(response);
  }
  // if (mode == AUTO)
  // {
  //   autoMode();
  // }

  // lcd.printBlower();
  // lcd.printPump();
  // lcd.printLed();

  // lcd.blowerSlider(random(0, 100));
  // lcd.ledSlider(random(0, 100));
  // lcd.pumpSlider(random(0, 100));
}

void autoMode()
{
  // if (data.temperature > 30)
  // {
  //   Serial.println("Temperature is too high, turning on blower");
  //   Serial2.print("blower.val=1");
  //   endNextionCommand();
  // }
  // else
  // {
  //   Serial.println("Temperature is normal, turning off blower");
  //   Serial2.print("blower.val=0");
  //   endNextionCommand();
  // }
}

void assignCommand(char response[])
{
  if (String(response[0]) + String(response[1]) == "00")
  {
    String asciiString = String(response[3]) + String(response[4]);
    String result = "blower,";
    for (int i = 0; i < asciiString.length(); i++)
    {
      int originalValue = (int)asciiString[i];
      int mappedValue = (originalValue - 1) * 255 / 99;
      String(mappedValue);
      result += String((int)asciiString[i]) + ";";
    }
    Serial.println(result);
    myPort.println(result);
  }
  else if (String(response[0]) + String(response[1]) == "01")
  {
    String asciiString = String(response[3]) + String(response[4]);
    String result = "pump,";
    for (int i = 0; i < asciiString.length(); i++)
    {
      result += String((int)asciiString[i]) + ";";
    }
    Serial.println(result);
    myPort.println(result);
  }
  else if (String(response[0]) + String(response[1]) == "02")
  {
    String asciiString = String(response[3]) + String(response[4]);
    String result = "led,";
    for (int i = 0; i < asciiString.length(); i++)
    {
      result += String((int)asciiString[i]) + ";";
    }
    Serial.println(result);
    myPort.println(result);
  }
  else if (String(response[0]) + String(response[1]) == "03")
  {
    String asciiString = String(response[3]) + String(response[4]);
    String result = "Auto";
    mode = AUTO;
    Serial.println(result);
  }
}

bool readSensor()
{
  data.age = doc["age"].isNull() ? data.age : doc["age"];
  data.humidity = doc["humidity"].isNull() ? data.humidity : doc["humidity"];
  data.temperature = doc["temperature"].isNull() ? data.temperature : doc["temperature"];
  data.ammonia = doc["ammonia"].isNull() ? data.ammonia : random(0, 100) / 10.0F;
  data.lux = doc["light_intensity"].isNull() ? data.lux : doc["light_intensity"];
  data.windSpeed = doc["wind_speed"].isNull() ? data.windSpeed : doc["wind_speed"];

  if (!doc["age"].isNull() || !doc["humidity"].isNull() || !doc["temperature"].isNull() || !doc["light_intensity"].isNull() || !doc["wind_speed"].isNull())
  {
    return true;
  }
  return false;
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  deserializeJson(doc, messageTemp);
}

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("farm/+/sensor"))
    {
      Serial.println("connected");
      client.subscribe("farm/+/sensor");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}