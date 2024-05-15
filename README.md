# ESP32 Weather Station with MQTT

This repository contains the source code for an ESP32-based weather station. The project leverages a WiFi connection to publish temperature and humidity readings to an MQTT broker and serves a webpage displaying live sensor data with graphical charts.

## Features

- WiFi connectivity to connect to local networks.
- MQTT integration for data publishing.
- Live web server hosting an HTML interface showing temperature and humidity data.
- Real-time graphical display of sensor readings.

## Prerequisites

### Hardware

- ESP32 Development Board
- AHT20 Temperature and Humidity Sensor

### Software

- Arduino IDE
- Libraries:
  - `WiFi.h`
  - `PubSubClient.h`
  - `Adafruit_AHTX0.h`
  - `AsyncTCP.h`
  - `ESPAsyncWebServer.h`

## Installation

1. **Clone the Repository**
   ```bash
   git clone https://github.com/Jordy-Hermes/5IOT.git

2. **Install Dependencies**
    Open the Arduino IDE, go to Sketch > Include Library > Manage Libraries. Install the following libraries:
        WiFi
        PubSubClient
        Adafruit AHTX0 Library
        AsyncTCP
        ESPAsyncWebServer

3. **Configure WiFi and MQTT**
    Edit the ssid, password, and mqtt_server variables in the code to match your network settings and MQTT broker.

## Usage

1. **Compile and Upload**
    Connect your ESP32 to your computer, select the correct board under Tools > Board, and press Upload.

2. **Access the Web Interface**
    Once the device is running, open a web browser and go to the IP address shown in the serial monitor to view the sensor data.

3. **MQTT Monitoring**
    Subscribe to the jordy/temperature and jordy/humidity topics in your MQTT client to receive live updates.