#include "BME280.h"
#include "bme280_registers.h"
#include "esp_log.h"
#define TAG "BME280"

static uint16_t dig_T1;
static int16_t dig_T2;
static int16_t dig_T3;

static uint8_t dig_H1;
static int16_t dig_H2;
static uint8_t dig_H3;
static int16_t dig_H4;
static int16_t dig_H5;
static int8_t dig_H6;

static int32_t t_fine;

esp_err_t bme280_hand_shake(void){
    esp_err_t ret = ESP_OK;
    uint8_t id;
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CHIP_ID, &id, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read BME280 register");
        return ret;
    }
    if (id != BME280_CHIP_ID) {
        ESP_LOGE(TAG, "Got unexpected BME280 chip id: Expected: 0x%X got: 0x%X",
                 BME280_CHIP_ID, id);
        return ESP_FAIL;
    } 
    return ESP_OK;
}

static esp_err_t read_bme280_calibration_data(void) {
    esp_err_t ret;
    uint8_t calib_data[24]; 
    
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, 0x88, calib_data, 24);
    if (ret != ESP_OK) return ret;
    
    dig_T1 = (uint16_t)((calib_data[1] << 8) | calib_data[0]);
    dig_T2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    dig_T3 = (int16_t)((calib_data[5] << 8) | calib_data[4]);
    
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, 0xA1, &dig_H1, 1);
    if (ret != ESP_OK) return ret;
    
    uint8_t hum_calib[7];
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, 0xE1, hum_calib, 7);
    if (ret != ESP_OK) return ret;
    
    dig_H2 = (int16_t)((hum_calib[1] << 8) | hum_calib[0]);
    dig_H3 = hum_calib[2];
    dig_H4 = (int16_t)((hum_calib[3] << 4) | (hum_calib[4] & 0x0F));
    dig_H5 = (int16_t)((hum_calib[5] << 4) | (hum_calib[4] >> 4));
    dig_H6 = (int8_t)hum_calib[6];
    
    ESP_LOGI(TAG, "Calibration data: T1=%u, T2=%d, T3=%d", dig_T1, dig_T2, dig_T3);
    ESP_LOGI(TAG, "Humidity calibration: H1=%u, H2=%d, H3=%u, H4=%d, H5=%d, H6=%d", 
             dig_H1, dig_H2, dig_H3, dig_H4, dig_H5, dig_H6);
    
    return ESP_OK;
}

esp_err_t init_bme280(void) {
    esp_err_t ret = bme280_hand_shake();
    if (ret != ESP_OK) return ret;
    
    uint8_t write_data = BME280_RESET_VALUE;
    ret = i2c_write_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_RESET, &write_data, 1);
    if (ret != ESP_OK) return ret;
    
    vTaskDelay(pdMS_TO_TICKS(10));
    
    ret = read_bme280_calibration_data();
    if (ret != ESP_OK) return ret;
    
    write_data = BME280_OSRS_H_1X;
    ret = i2c_write_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CTRL_HUM, &write_data, 1);
    if (ret != ESP_OK) return ret;
    
    write_data = (BME280_OSRS_T_1X | BME280_OSRS_P_1X | BME280_MODE_NORMAL);
    ret = i2c_write_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CTRL_MEAS, &write_data, 1);
    if (ret != ESP_OK) return ret;
    
    vTaskDelay(pdMS_TO_TICKS(10));
    
    return ESP_OK;
}

static float compensate_temperature(int32_t adc_T) {
    int32_t var1, var2;
    
    ESP_LOGI(TAG, "Raw temperature ADC value: %ld (0x%lx)", adc_T, adc_T);
    
    if (adc_T == 0x80000) {
        ESP_LOGE(TAG, "Temperature reading was skipped or invalid");
        return 0.0f;
    }
    
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    
    ESP_LOGI(TAG, "Temperature compensation: var1=%ld, var2=%ld", var1, var2);
    
    t_fine = var1 + var2;
    ESP_LOGI(TAG, "t_fine=%ld", t_fine);
    
    float T = (t_fine * 5 + 128) >> 8;
    T = T / 100.0f;
    
    ESP_LOGI(TAG, "Compensated temperature: %.2f °C", T);
    return T;
}

// Humidity compensation formula based on BME280 datasheet
static float compensate_humidity(int32_t adc_H) {
    int32_t v_x1_u32r;
    
    if (t_fine == 0) {
        ESP_LOGE(TAG, "t_fine is zero, temperature must be read first");
        return 0.0f;
    }
    
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + 
                 ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * 
                 (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + 
                 ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
    
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    
    return (float)(v_x1_u32r >> 12) / 1024.0f;
}

esp_err_t bme280_read_temperature(float* temp) {
    esp_err_t ret = ESP_OK;
    uint8_t data[3];
    int32_t adc_T;
    
    uint8_t ctrl_meas;
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read control register");
        return ret;
    }
    
    ctrl_meas = (ctrl_meas & 0xFC) | BME280_MODE_FORCED;
    ret = i2c_write_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set forced mode");
        return ret;
    }
    
    uint8_t status;
    do {
        vTaskDelay(pdMS_TO_TICKS(10)); // Wait a bit
        ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_STATUS, &status, 1);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read status");
            return ret;
        }
    } while (status & (1 << BME280_STATUS_MEASURING_BIT));
    
    // Read all 3 temperature registers (MSB, LSB, XLSB) at once
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_TEMP_MSB, data, 3);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read temperature data!");
        return ret;
    }
    
    // Print raw bytes for debugging
    ESP_LOGI(TAG, "Raw temperature bytes: 0x%02X, 0x%02X, 0x%02X", data[0], data[1], data[2]);
    
    adc_T = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4);
    
    *temp = compensate_temperature(adc_T);
    
    return ESP_OK;
}

esp_err_t bme280_read_humidity(float* humidity) {
    esp_err_t ret = ESP_OK;
    uint8_t data[2];
    int32_t adc_H;
    float temp;
    
    if (t_fine == 0) {
        ret = bme280_read_temperature(&temp);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    
    ret = i2c_read_register(BME280_I2C_ADDR_PRIMARY, BME280_REG_HUM_MSB, data, 2);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read humidity data!");
        return ret;
    }
    
    ESP_LOGI(TAG, "Raw humidity bytes: 0x%02X, 0x%02X", data[0], data[1]);
    
    adc_H = ((uint32_t)data[0] << 8) | data[1];
    ESP_LOGI(TAG, "Raw humidity ADC value: %ld", adc_H);
    
    *humidity = compensate_humidity(adc_H);
    
    if (*humidity > 100.0f) *humidity = 100.0f;
    if (*humidity < 0.0f) *humidity = 0.0f;
    
    return ESP_OK;
}
