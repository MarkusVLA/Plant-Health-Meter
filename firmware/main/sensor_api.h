// This file containse getters for reading sensor data: API for sensor data
//

#pragma once
#include "packet.h"

// get raw sensor reading from moisture sensor    
// Moisture sensor reading has ~80ms settling time after power up
uint16_t get_moisture_sensor_value(void);


// Get data packet containing all sensor readings
sensor_data_packet_t get_sensor_data_packet(void);
