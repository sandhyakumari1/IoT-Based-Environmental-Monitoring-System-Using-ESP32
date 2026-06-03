# IoT-Based Environmental Monitoring System Using ESP32

## Overview

This project presents an IoT-based Environmental Monitoring System designed to monitor environmental parameters such as temperature, humidity, and atmospheric pressure in real time. The system uses an ESP32 microcontroller and environmental sensors to collect data and transmit it to cloud platforms for remote monitoring and analysis.

## Features

* Real-time monitoring of environmental parameters
* Temperature measurement using DHT11 sensor
* Humidity monitoring
* Atmospheric pressure measurement using BMP085 sensor
* Wireless data transmission using GSM Module
* Cloud-based data visualization and storage
* Remote access to sensor data through IoT platforms
* Low-cost and energy-efficient design

## Hardware Components

* ESP32 Development Board
* DHT11 Temperature and Humidity Sensor
* BMP085 Pressure Sensor
* OLED Display
* GSM Module (for SMS alerts)
* LoRa Module (optional long-range communication)
* Power Supply

## Software and Tools

* Arduino IDE
* ESP32 Board Package
* Embedded C/C++
* ThingSpeak Cloud Platform
* IoT Communication Protocols

## System Architecture

1. Environmental sensors collect real-time data.
2. ESP32 processes sensor readings.
3. Data is displayed locally on the OLED screen.
4. Sensor data is transmitted to the cloud using GSM Module.
5. Users can monitor environmental conditions remotely using the cloud dashboard.
6. Alert notifications can be sent when predefined thresholds are exceeded.


## Installation and Setup

### Hardware Setup

1. Connect DHT11 and BMP085 sensors to the ESP32.
2. Connect the OLED display and communication modules.
3. Power the ESP32 board.

### Software Setup

1. Install Arduino IDE.
2. Install ESP32 board support package.
3. Install required sensor libraries.
4. Upload the source code to the ESP32.
5. Configure Wi-Fi credentials and cloud platform settings.

## Applications

* Smart Agriculture
* Weather Monitoring Stations
* Smart Cities
* Industrial Environmental Monitoring
* Indoor Air Quality Monitoring
* Remote Sensing Applications

## Future Enhancements

* Integration of additional environmental sensors
* Mobile application support
* Machine learning-based anomaly detection
* Real-time alert notifications
* Energy optimization using sleep modes
* Dashboard analytics and reporting

## Results

The developed system successfully monitors environmental parameters in real time and uploads the collected data to a cloud platform, enabling remote observation and analysis.

## Author

Sandhya Kumari

M.Tech (Communication & Networks)

National Institute of Technology Rourkela
