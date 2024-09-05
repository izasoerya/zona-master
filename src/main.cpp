#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <regex> // Include the regex library

#include "models.h"

// WiFi credentials
const char *ssid = "Subhanallah";
const char *password = "muhammadnabiyullah";

// MQTT broker details
const char *mqtt_server = "zona-api.theunra.site";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
SensorData data;
JsonDocument doc;
RunMode mode = NORMAL;

void sendHumidityToNextion();
void readSensor();
void sendAgeToNextion();
void sendHumidityToNextion();
void sendTemperatureToNextion();
void sendAmmoniaToNextion();
void sendLuxToNextion();
void sendWindSpeedToNextion();
void endNextionCommand();
void setup_wifi();
void reconnect();
void assignCommand(char response[]);
void callback(char *topic, byte *message, unsigned int length);
void autoMode();

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);

  randomSeed(analogRead(0));

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
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

void loop()
{
  if (!client.connected())
    reconnect();
  client.loop();

  readSensor();
  sendAgeToNextion();
  sendHumidityToNextion();
  sendTemperatureToNextion();
  sendAmmoniaToNextion();
  sendLuxToNextion();
  sendWindSpeedToNextion();

  char response[100] = "";

  while (Serial2.available())
  {
    String responseString = Serial2.readString();
    Serial.println("data: " + responseString);
    String result = findPattern(responseString);
    if (result.length() > 0)
    {
      result.trim();
      strncpy(response, result.c_str(), sizeof(response) - 1);
      response[sizeof(response) - 1] = '\0'; // Ensure null-termination
      break;                                 // Exit the loop after finding the pattern
    }
  }
  if (response[0] != 0)
  {
    Serial.println(response);
    assignCommand(response);
  }
  if (mode == AUTO)
  {
    autoMode();
  }
  String command = "h0.val=";
  Serial2.print(command + 'd');
  endNextionCommand();
  delay(1);
}

void autoMode()
{
  if (data.temperature > 30)
  {
    Serial.println("Temperature is too high, turning on blower");
    Serial2.print("blower.val=1");
    endNextionCommand();
  }
  else
  {
    Serial.println("Temperature is normal, turning off blower");
    Serial2.print("blower.val=0");
    endNextionCommand();
  }
}

void assignCommand(char response[])
{
  if (String(response[0]) + String(response[1]) == "00")
  {
    String asciiString = String(response[3]) + String(response[4]);
    String result = "Blower";
    for (int i = 0; i < asciiString.length(); i++)
    {
      result += String((int)asciiString[i]) + " ";
    }
    Serial.println(result);
  }
  else if (String(response[0]) + String(response[1]) == "01")
  {
    String asciiString = String(response[3]) + String(response[4]);
    String result = "Pump";
    for (int i = 0; i < asciiString.length(); i++)
    {
      result += String((int)asciiString[i]) + " ";
    }
    Serial.println(result);
  }
  else if (String(response[0]) + String(response[1]) == "02")
  {
    String asciiString = String(response[3]) + String(response[4]);
    String result = "Lampu";
    for (int i = 0; i < asciiString.length(); i++)
    {
      result += String((int)asciiString[i]) + " ";
    }
    Serial.println(result);
  }
  else if (String(response[0]) + String(response[1]) == "03")
  {
    String asciiString = String(response[3]) + String(response[4]);
    String result = "Auto";
    mode = AUTO;
    Serial.println(result);
  }
}

void readSensor()
{
  data.age = doc["age"].isNull() ? data.age : doc["age"];
  data.humidity = doc["humidity"].isNull() ? data.humidity : doc["humidity"];
  data.temperature = doc["temperature"].isNull() ? data.temperature : doc["temperature"];
  data.ammonia = doc["ammonia"].isNull() ? data.ammonia : doc["ammonia"];
  data.lux = doc["light_intensity"].isNull() ? data.lux : doc["light_intensity"];
  data.windSpeed = doc["wind_speed"].isNull() ? data.windSpeed : doc["wind_speed"];
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

void sendAgeToNextion()
{
  String command = "umur.txt=\"" + String(data.age) + "\"";
  Serial2.print(command);
  endNextionCommand();
}

void sendHumidityToNextion()
{
  String command = "humidity.txt=\"" + String(data.humidity) + "\"";
  Serial2.print(command);
  endNextionCommand();
}

void sendTemperatureToNextion()
{
  String command = "suhu.txt=\"" + String(data.temperature, 1) + "\"";
  Serial2.print(command);
  endNextionCommand();
}

void sendAmmoniaToNextion()
{
  String command = "amonia.txt=\"" + String(data.ammonia) + "\"";
  Serial2.print(command);
  endNextionCommand();
}

void sendLuxToNextion()
{
  String command = "lux.txt=\"" + String(data.lux) + "\"";
  Serial.println("lux:" + String(data.lux));
  Serial2.print(command);
  endNextionCommand();
}

void sendWindSpeedToNextion()
{
  String command = "wind.txt=\"" + String(data.windSpeed) + "\"";
  Serial2.print(command);
  endNextionCommand();
}

void endNextionCommand()
{
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
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