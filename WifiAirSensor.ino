#include "WifiCredentials.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void setup_wifi()
{
  delay(10);
  // Connect to a WiFi network
  Serial.println();
  Serial.print("Connecting to WiFi network");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *out_topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(out_topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  }
  else
  {
    digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      StaticJsonDocument<256> announceDoc;
      announceDoc["messageType"] = "announcement";
      char buffer[256];
      size_t n = serializeJson(announceDoc, buffer);
      client.publish(out_topic, buffer, n);
      serializeJson(announceDoc, Serial);
      Serial.println();
      // ... and resubscribe
      client.subscribe(in_topic);
    }
    else
    {
      Serial.print("FAILED | state = ");
      Serial.print(client.state());
      Serial.println(" | try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 10000)
  {
    lastMsg = now;
    StaticJsonDocument<256> doc;
    doc["messageType"] = "sensorReading";
    doc["sensorReading"] = analogRead(0);
    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    client.publish(out_topic, buffer, n);
    serializeJson(doc, Serial);
    Serial.println();
  }
}