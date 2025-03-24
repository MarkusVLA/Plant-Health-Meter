#include "packet.h"

int packet_to_json(char* buffer, size_t buffer_size, const sensor_data_packet_t* packet) {
    return snprintf(buffer, buffer_size, 
                   "{\"moisture\":%.2f,\"light\":%.2f,\"temperature\":%.2f,\"humidity\":%.2f}", 
                   packet->moisture, packet->light, packet->temperature, packet->humidity);
}
