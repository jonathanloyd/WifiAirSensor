#include <ESP8266WiFi.h>
#include "WifiCredentials.h"

void setup()
{
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{

}
