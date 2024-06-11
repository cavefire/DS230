#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <PubSubClient.h>
#include "config.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void connectWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
  }
}

void connectMQTT()
{
  client.setServer(BROKER, PORT);
  while (!client.connected())
  {
    client.connect(CLIENT_ID, USERNAME, PASSWORD);
  }
}

void setup()
{
  Serial.begin(57600);
  connectWiFi();
  connectMQTT();
}

void loop()
{
  if (Serial.available() > 0)
  {
    String message = Serial.readStringUntil('\n');
    int eventTypeIndex = message.indexOf("snapshot with event_type:");
    if (eventTypeIndex != -1)
    {
      int eventTypeStart = eventTypeIndex + 25;
      int eventTypeEnd = message.indexOf(" ", eventTypeStart);
      String event_type = message.substring(eventTypeStart, eventTypeEnd);
      if (WiFi.status() != WL_CONNECTED)
      {
        connectWiFi();
      }
      if (!client.connected())
      {
        connectMQTT();
      }
      client.publish(BASE_TOPIC "event_type", event_type.c_str());
    }
  }
}