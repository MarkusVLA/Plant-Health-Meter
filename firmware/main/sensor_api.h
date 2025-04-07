// This file containse getters for reading sensor data: API for sensor data

#pragma once
#include "packet.h"
#include <esp_err.h>

// get sensor reading from moisture sensor    
// Moisture sensor reading has ~80ms settling time after power up
float get_moisture_sensor_value(void);

// Get the temperature sensor reading
float get_temperature_sensor_value(void);

// Intialize all sensors and get status.
esp_err_t init_sensors(void);

// Get data packet containing all sensor readings
sensor_data_packet_t get_sensor_data_packet(void);
