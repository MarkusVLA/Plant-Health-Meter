#include "i2c_master.h"
#include "esp_log.h"

#define TAG "I2C driver"

esp_err_t begin_i2c(void) {

    esp_err_t ret;
    
    i2c_config_t i2c_config = {
        .mode=I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_SCL_FREQ 
    };
     
    ret = i2c_param_config(I2C_PORT, &i2c_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load i2c config");
        return ret;
    }

    ret = i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install i2c driver");
        return ret;
    }
    
    return ret;
}

esp_err_t i2c_write_register(uint8_t i2c_addr, uint8_t reg_addr, 
                             uint8_t *data, size_t data_size){

    esp_err_t ret = ESP_OK;
    // Create commands
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2c_addr << 1), 0x01);
    i2c_master_write_byte(cmd, reg_addr, 0x01);
    i2c_master_write(cmd, data, data_size, 0x01);
    i2c_master_stop(cmd);
    // Write command sequence to bus and check for errors
    ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
