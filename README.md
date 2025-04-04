## Plant-health-meter
This repository contains Aalto AAN-C2009 project device hardware configuration and embedded software for a plant health monitoring system based on the ESP32c3 SoC.

## Features
- Soil moisture sensing
- Ambient light monitoring
- Temperature and humidity measurement
- Battery-powered operation with charging circuit
- Web interface for monitoring plant health 

## PCB
PCB design done in KiCad.
![alt tex](PCB/img/3d.png)
![alt tex](PCB/img/layout.png)
Schematic: [PDF](PCB/img/schematic.pdf)

## Key Components
- ESP32-C3 microcontroller
- BME280 temperature/humidity sensor
- Custom capacitive soil moisture sensor
- Photodiode and OPA340 based transimpedance amplifier
- ADS1115 16-bit ADC for accurate measurements

## Embedded software
Embedded software is written in C using the ESP-IDF (Espressif IoT Development Framework)
[ESP programming guide](https://docs.espressif.com/projects/esp-idf/en/v5.2.5/esp32c3/get-started/index.html)

## Pin Assignments
See the schematic for detailed I/O connections and sensor bus configuration.

## Setup Instructions
1. Follow the ESP-IDF installation guide
2. Clone this repository
3. In the `main/` folder create `private_config.h` file, with following definitions:
    1. `#define FIREBASE_HOST "xxx.firebasedatabase.app"` - url of the firebase realtime database
    2. `#define FIREBASE_AUTH ""` - authentication token for the firebase - (TODO: not tested yet with protected database)
4. Set ESP32-C3 target in the idf `idf.py set-target esp32c3`
5. Configure the partitions and BLE version `idf.py menuconfig`:
    1. Go to Partition Table → Partition Table and select Custom partition table CSV. Then set the path to `partitions.csv`.
    2. Go to Component Config → Bluetooth → Bluedroid BLE version and choose v4.0.
    3. Save and exit the configuration menu.
6. Build and flash using `idf.py build flash monitor`

## Connection Instructions

1. By default, there are no Wi-Fi credentials set. The device will immediately enter Bluetooth setup mode on boot.

2. For setup, you’ll need a mobile app capable of sending data over BLE, such as **nRF Connect**.

3. When the device is in Bluetooth setup mode, one of its LEDs will begin blinking. You can then find the device in the app under the name:  
   **`PLANT_SENSOR_01`**

4. Press **Connect** to establish a Bluetooth connection.

5. Once connected, look for **Advertised Services** in the device details. Find the characteristic that supports both **Read** and **Write**, and tap the **up arrow (⬆️)** to write data.

6. Select **UTF-8** as the encoding format and send a message with the following structure:  
   ```
   device_id:wifi_ssid:wifi_password
   ```

   For example:  
   ```
   sensor1:mywifi:secret_password
   ```

7. After sending the credentials, the device will restart and attempt to connect to the specified Wi-Fi network.

8. Once connected, sensor data will be sent to Firebase under the provided `device_id`.

---

**Important Notes:**
- `device_id` can contain only **letters, digits, and underscores** (e.g., `plant_sensor_1`).
- If the device fails to connect to Wi-Fi, it will re-enter Bluetooth setup mode on reboot.


## Power Requirements
3.7V Li-ion battery or USB power
