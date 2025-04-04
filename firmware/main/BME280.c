#include "BME280.h"
#include "bme280_registers.h"
#include "esp_log.h"

#define TAG "BME280"

static uint16_t dig_T1;
static uint16_t dig_T2;
static uint16_t dig_T3;

esp_err_t bme280_hand_shake(void){
    // Read the bme280 id register to verify that the device is working properly.
    // Contains 1 byte identifier. 
    esp_err_t ret = ESP_OK;
    uint8_t id;
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CHIP_ID, &id, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read BME280 register");
        return ret;
    }
    // check that we got the expected id byte.
    if (id != BME280_CHIP_ID) {
        ESP_LOGE(TAG, "Got unexpected BME280 chip id: Expected: 0x%X got: 0x%X",
                 BME280_CHIP_ID, id);
        return ESP_FAIL;
    } 
    return ESP_OK;
}

static esp_err_t read_bme280_calibration_data(void) {
    uint8_t calib_data[6];
    esp_err_t ret;
    
    // Read temperature calibration data from 0x88-0x8D
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CALIB00, calib_data, 6);
    if (ret != ESP_OK) return ret;
    
    dig_T1 = (uint16_t)(calib_data[1] << 8 | calib_data[0]);
    dig_T2 = (int16_t)(calib_data[3] << 8 | calib_data[2]);
    dig_T3 = (int16_t)(calib_data[5] << 8 | calib_data[4]);
    
    return ESP_OK;
}

esp_err_t init_bme280() {
    esp_err_t ret = bme280_hand_shake();
    if (ret != ESP_OK) return ret;
    
    // Reset the device
    uint8_t write_data = BME280_RESET_VALUE;
    ret = i2c_write_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_RESET, &write_data, 1);
    if (ret != ESP_OK) return ret;
    
    // Wait 2ms after reset
    vTaskDelay(pdMS_TO_TICKS(2));
    
    // Configure humidity oversampling (must be set before CTRL_MEAS)
    write_data = BME280_OSRS_H_1X;
    ret = i2c_write_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CTRL_HUM, &write_data, 1);
    if (ret != ESP_OK) return ret;
    
    // Configure temperature/pressure oversampling and mode
    write_data = (BME280_OSRS_T_1X | BME280_OSRS_P_1X | BME280_MODE_NORMAL);
    ret = i2c_write_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CTRL_MEAS, &write_data, 1);
    if (ret != ESP_OK) return ret;

    ret = read_bme280_calibration_data();
    if (ret != ESP_OK) return ret;
    
    return ESP_OK;
}


esp_err_t bme280_read_temperature(uint16_t* temp) {
    esp_err_t ret = ESP_OK;
    uint8_t ctrl_meas_reg, status;
    uint8_t temp_msb = 0x00;
    uint8_t temp_lsb = 0x00;
    
    // Read current control settings
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CTRL_MEAS, &ctrl_meas_reg, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read control register!");
        return ret;
    }
    
    // Trigger a measurement in forced mode
    ctrl_meas_reg = (ctrl_meas_reg & 0xFC) | BME280_MODE_FORCED;
    ret = i2c_write_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CTRL_MEAS, &ctrl_meas_reg, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start measurement!");
        return ret;
    }
    
    do {
        vTaskDelay(pdMS_TO_TICKS(5)); 
        ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_STATUS, &status, 1);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read status register!");
            return ret;
        }
    } while (status & (1 << BME280_STATUS_MEASURING_BIT)); // Wait while measuring
    
    // Read temperature MSB and LSB
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_TEMP_MSB, &temp_msb, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read temperature MSB!");
        return ret;
    }
    
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_TEMP_LSB, &temp_lsb, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read temperature LSB!");
        return ret;
    }
    
    *temp = (temp_msb << 8) | temp_lsb;
    
    return ESP_OK;
}





