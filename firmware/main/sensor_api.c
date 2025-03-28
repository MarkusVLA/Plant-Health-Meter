#include "sensor_api.h"


sensor_data_packet_t get_sensor_data_packet(void){
    // Example dummy packet. Real data later
    sensor_data_packet_t ret = {
        .humidity = 50.2,
        .temperature = 22.3,
        .light = 40.1,
        .moisture = 33.0,
        .header = 0x00
    };
    
    // copy of generated packet
    return ret;
} 

