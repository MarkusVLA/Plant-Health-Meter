#include "sensor_api.h"
#include <math.h>
// Placeholder function

sensor_data_packet_t get_sensor_data_packet(void){
    // Example dummy packet. Real data later
    sensor_data_packet_t ret = {
        .humidity = 12,
        .temperature = 32,
        .light = 122,
        .moisture = 22,
        .header = 0x00
    };
    
    // copy of generated packet
    return ret;

} 
