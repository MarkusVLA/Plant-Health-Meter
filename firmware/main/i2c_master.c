#include "i2c_master.h"
#include "esp_log.h"
#include <string.h>

#define TAG "I2C driver"

esp_err_t begin_i2c(void) {

    esp_err_t ret;
    
    i2c_config_t i2c_config = {
        .mode=I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_SCL_FREQ 
    };
     
    ret = i2c_param_config(I2C_PORT_NUM, &i2c_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load i2c config");
        return ret;
    }

    ret = i2c_driver_install(I2C_PORT_NUM, I2C_MODE_MASTER, 0, 0, 0);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install i2c driver");
        return ret;
    }
    
    return ret;
}

static esp_err_t i2c_write_register(uint8_t i2c_addr, uint8_t reg_addr, 
                             uint8_t *data, size_t data_size){

    esp_err_t ret = ESP_OK;
    // Create commands
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // Ignore response for testing purposes
    i2c_master_write_byte(cmd, (i2c_addr << 1) | 0x00, IGNORE_RESPONSE); // Write bit 0
    i2c_master_write_byte(cmd, reg_addr, IGNORE_RESPONSE);
    i2c_master_write(cmd, data, data_size, IGNORE_RESPONSE);
    i2c_master_stop(cmd);
    // Write command sequence to bus and check for errors
    ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 100 / portTICK_PERIOD_MS);

    if (ret != ESP_OK){
        ESP_LOGE(TAG, "Error writing i2c command to port");
    }
    
    i2c_cmd_link_delete(cmd);
    return ret;
}


static esp_err_t i2c_read_register(uint8_t i2c_addr, uint8_t reg_addr, 
                             uint8_t *data, size_t data_size){
    
    esp_err_t ret = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // write the register address
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2c_addr << 1) | 0x00, IGNORE_RESPONSE); 
    i2c_master_write_byte(cmd, reg_addr, IGNORE_RESPONSE);  
    
    i2c_master_start(cmd);  // Repeated start condition
    i2c_master_write_byte(cmd, (i2c_addr << 1) | 0x01, IGNORE_RESPONSE);  // read bit 1 
    
    if (data_size > 1) {
        i2c_master_read(cmd, data, data_size - 1, I2C_MASTER_ACK);  // Read all but last byte with ACK
    }
    
    // Last byte with NACK
    i2c_master_read_byte(cmd, data + data_size - 1, I2C_MASTER_NACK);  
    
    i2c_master_stop(cmd);
    
    ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 100 / portTICK_PERIOD_MS);
    if (ret != ESP_OK){
        ESP_LOGE(TAG, "Error reading i2c command from port");
    }
    
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t test_i2c() {
    esp_err_t ret = ESP_OK;
    // Write arbitrary data to an arbitrary address then read.
    // Check with scope that both operations are recognised as R/W
    uint8_t test_data = 69;

    uint8_t* test_data_2 = (uint8_t*) malloc(sizeof(uint8_t) * 2);
    memset(test_data_2, 0xFF, 2);

    ret = i2c_write_register(0x73, 42, &test_data, 1);
    ret = i2c_write_register(0x77, 42, test_data_2, 2);

    free(test_data_2);
    return ret;
}
















