#include "packet.h"

int packet_to_json(char* buffer, size_t buffer_size, const sensor_data_packet_t* packet) {
    return snprintf(buffer, buffer_size, 
                   "{\"moisture\":%d,\"light\":%d,\"temperature\":%d,\"humidity\":%d}", 
                   packet->moisture, packet->light, packet->temperature, packet->humidity);
}
