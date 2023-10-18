# WifiAirSensor

Original plan for this project was to make a sensor that connects to Azure IoT hub or IoT central and send data to SQL.
Azure IoT Hub and DPS have recently updated their TLS certificates and libraries for the ESP8266 board have yet to be updated.
The direction of this project is changing to connect via MQTT using PubSuBClient and send the data to a SQL server that is TBD.
