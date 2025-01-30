#include "i2c_master.h"



esp_err_t begin_i2c(void) {
    i2c_master_bus_config_t i2c_master_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = GPIO_NUM_5, // SCL and SDA outputs based on KiCad schematic
        .sda_io_num = GPIO_NUM_4,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false, // Board will have external 4.7k
                                               // resistors on SDA and SCL lines
    };

    i2c_master_bus_handle_t bus_handle; 
    


}

