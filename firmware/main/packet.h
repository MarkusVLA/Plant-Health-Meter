// Network packet structure definitions.
//
#pragma once
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t header;
    float moisture, light, temperature, humidity;
} sensor_data_packet_t;

int packet_to_json(char* buffer, size_t buffer_size, const sensor_data_packet_t* packet);
