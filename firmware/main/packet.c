#include "packet.h"

int packet_to_json(char* buffer, size_t buffer_size, const sensor_data_packet_t* packet) {
    return snprintf(buffer, buffer_size, 
                   "{\"moisture\":%.4f,\"light\":%.4f,\"temperature\":%.4f,\"humidity\":%.4f}", 
                   packet->moisture, packet->light, packet->temperature, packet->humidity);
}
