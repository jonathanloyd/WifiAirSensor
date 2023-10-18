#include "WifiCredentials.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <AzureIoTHubMQTTClient.h>

const char *AP_SSID = SSID;
const char *AP_PASS = PASSWORD;

WiFiClientSecure tlsClient;
AzureIoTHubMQTTClient client(tlsClient, IOTHUB_HOSTNAME, DEVICE_ID, DEVICE_KEY);
WiFiEventHandler e1, e2;

unsigned long lastMillis = 0;

void connectToIoTHub()
{
  Serial.print("\nBeginning Azure IoT Hub Client... ");
  if (client.begin())
  {
    Serial.println("Connected to MQTT");
  }
  else
  {
    Serial.println("Could not connect to MQTT");
  }
}

void onMessageCallback(const MQTT::Publish &msg)
{
  // Handle Cloud to Device message

  //  if (msg.payload_len() == 0) {
  //      return;
  //  }

  //  Serial.println(msg.payload_string());
}

void onSTAGotIP(WiFiEventStationModeGotIP ipInfo)
{
  Serial.printf("Current IP: %s\r\n", ipInfo.ip.toString().c_str());

  // Connect to IoT Hub upon WiFi connected
  connectToIoTHub();
}

void onSTADisconnected(WiFiEventStationModeDisconnected event_info)
{
  Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
  Serial.printf("Reason: %d\n", event_info.reason);
}

void onClientEvent(const AzureIoTHubMQTTClient::AzureIoTHubMQTTClientEvent event)
{
  if (event == AzureIoTHubMQTTClient::AzureIoTHubMQTTClientEventConnected)
  {
    Serial.println("Connected to Azure IoT Hub");

    // Add the callback to process cloud-to-device message/command
    client.onMessage(onMessageCallback);
  }
}

void onActivateRelayCommand(String cmdName, JsonVariant jsonValue)
{
  // Parse cloud-to-device message JSON
  // {"Name":"ActivateRelay","Parameters":{"Activated":0}}

  // JsonObject& jsonObject = jsonValue.as<JsonObject>();
  // if (jsonObject.containsKey("Parameters"))
  // {
  //   auto params = jsonValue["Parameters"];
  //   auto isAct = (params["Activated"]);
  //   if (isAct)
  //   {
  //     Serial.println("Activated true");
  //   }
  //   else
  //   {
  //     Serial.println("Activated false");
  //   }
  // }
}

void setup()
{
  Serial.begin(115200);

  while (!Serial)
  {
    yield();
  }
  delay(2000);

  Serial.setDebugOutput(true);

  Serial.print("Connecting to WiFi...");
  // Begin WiFi joining with provided Access Point name and password
  WiFi.begin(AP_SSID, AP_PASS);

  // Handle WiFi events
  e1 = WiFi.onStationModeGotIP(onSTAGotIP); // As soon WiFi is connected, start the Client
  e2 = WiFi.onStationModeDisconnected(onSTADisconnected);

  // Handle client events
  client.onEvent(onClientEvent);

  // Command format is assumed like this: {"Name":"[COMMAND_NAME]","Parameters":[PARAMETERS_JSON_ARRAY]}
  client.onCloudCommand("ActivateRelay", onActivateRelayCommand);
}

void loop()
{
  // This MUST be in loop()
  client.run();

  if (client.connected())
  {
    // // Publish a message roughly every 3 second. Only after time is retrieved and set properly.
    // if (millis() - lastMillis > 3000 && timeStatus() != timeNotSet)
    // {
    //   lastMillis = millis();
      
    //   Serial.println("This is when we would send a message.");

    //   // // Read the actual temperature from sensor
    //   // float temp, press;
    //   // readSensor(&temp, &press);

    //   // Get current timestamp, using Time lib
    //   time_t currentTime = now();

    //   // // You can do this to publish payload to IoT Hub
    //   // /*
    //   // String payload = "{\"DeviceId\":\"" + String(DEVICE_ID) + "\", \"MTemperature\":" + String(temp) + ", \"EventTime\":" + String(currentTime) + "}";
    //   // Serial.println(payload);

    //   // //client.publish(MQTT::Publish("devices/" + String(DEVICE_ID) + "/messages/events/", payload).set_qos(1));
    //   // client.sendEvent(payload);
    //   // */

    //   // // Or instead, use this more convenient way
    //   // AzureIoTHubMQTTClient::KeyValueMap keyVal = {{"MTemperature", temp}, {"MPressure", press}, {"DeviceId", DEVICE_ID}, {"EventTime", currentTime}};
    //   // client.sendEventWithKeyVal(keyVal);
    // }
  }
  else
  {
    Serial.println("Client not connected");
  }

  delay(10); // Allow for WiFi stability
}
