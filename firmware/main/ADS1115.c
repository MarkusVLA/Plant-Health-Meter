#include "ADS1115.h"
#include "ADS1115_registers.h"
#include "esp_log.h"
#include "i2c_master.h"  
                         
#define ADS1115_I2C_ADDR ADS111X_I2C_ADDR_GND

static const char *TAG = "ADS1115";

#define ADS1115_CONVERSION_TIMEOUT_MS 200  

typedef struct {
    uint16_t config;
    uint8_t current_mux;
    uint16_t current_pga;
    bool is_continuous_mode;
} ads1115_state_t;

static ads1115_state_t ads1115_state = {
    .config = 0,
    .current_mux = 0,
    .current_pga = ADS111X_CFG_PGA_2_048V,
    .is_continuous_mode = false
};

// I2C helper functions
static esp_err_t i2c_write_reg(uint8_t reg_addr, uint16_t value) {
    uint8_t data[2];
    data[0] = (uint8_t)(value >> 8);    // MSB first (big-endian)
    data[1] = (uint8_t)(value & 0xFF);  // LSB
    
    return i2c_write_register(ADS1115_I2C_ADDR, reg_addr, data, 2);
}

static esp_err_t i2c_read_reg(uint8_t reg_addr, uint16_t *value) {
    uint8_t data[2];
    esp_err_t ret;
    
    ret = i2c_read_register(ADS1115_I2C_ADDR, reg_addr, data, 2);
    if (ret != ESP_OK) return ret;
    
    // Combine bytes (MSB first)
    *value = ((uint16_t)data[0] << 8) | data[1];
    
    return ESP_OK;
}

// Initialize the ADS1115
esp_err_t ads1115_init(void) {
    ESP_LOGI(TAG, "Initializing ADS1115");
    
    // Default configuration:
    // - Single-shot mode
    // - AIN0/GND input
    // - +/-4.096V range (default)
    // - 128 SPS (default)
    // - Disable comparator
    uint16_t config = ADS111X_CFG_OS_START |      // Start a single conversion
                      ADS111X_CFG_MUX_SINGLE_0 |  // Single-ended AIN0
                      ADS111X_CFG_PGA_4_096V |    // +/-4.096V range
                      ADS111X_CFG_MODE_SINGLE |   // Single-shot mode
                      ADS111X_CFG_DR_128SPS |     // 128 samples per second
                      ADS111X_CFG_CMODE_TRAD |    // Traditional comparator
                      ADS111X_CFG_CPOL_ACTVLOW |  // Active low
                      ADS111X_CFG_CLAT_NONLAT |   // Non-latching
                      ADS111X_CFG_CQUE_DISABLE;   // Disable comparator
    
    // Save the current configuration
    ads1115_state.config = config;
    ads1115_state.current_mux = (ADS111X_CFG_MUX_SINGLE_0 & ADS111X_CFG_MUX_MASK) >> ADS111X_CFG_MUX_SHIFT;
    ads1115_state.current_pga = ADS111X_CFG_PGA_4_096V;
    ads1115_state.is_continuous_mode = false;
    
    esp_err_t ret = i2c_write_reg(ADS111X_REG_CONFIG, config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write configuration register: %d", ret);
    }
    
    return ret;
}

// Get the current configuration
esp_err_t ads1115_get_config(uint16_t *config) {
    return i2c_read_reg(ADS111X_REG_CONFIG, config);
}

// Set configuration
esp_err_t ads1115_set_config(uint16_t config) {
    ads1115_state.config = config;
    return i2c_write_reg(ADS111X_REG_CONFIG, config);
}

// Set a specific bit field in the configuration register
esp_err_t ads1115_set_config_bits(uint16_t mask, uint16_t shift, uint16_t value) {
    uint16_t config;
    esp_err_t ret = ads1115_get_config(&config);
    if (ret != ESP_OK) return ret;
    
    // Clear the bits in the mask and set new value
    config &= ~mask;
    config |= (value << shift) & mask;
    
    return ads1115_set_config(config);
}

// Set the device mode (continuous or single-shot)
esp_err_t ads1115_set_mode(bool continuous) {
    uint16_t mode_value = continuous ? ADS111X_CFG_MODE_CONTIN : ADS111X_CFG_MODE_SINGLE;
    uint16_t config;
    
    // Get current config
    esp_err_t ret = ads1115_get_config(&config);
    if (ret != ESP_OK) return ret;
    
    // Clear mode bit and set it according to the parameter
    config &= ~ADS111X_CFG_MODE_MASK;
    config |= mode_value;
    
    // Set the config
    ret = ads1115_set_config(config);
    if (ret == ESP_OK) {
        ads1115_state.is_continuous_mode = continuous;
        // If switching to continuous mode, start a conversion
        if (continuous) {
            // Setting the OS bit starts a conversion
            config |= ADS111X_CFG_OS_START;
            ret = ads1115_set_config(config);
        }
    }
    
    return ret;
}

// Set the PGA gain
esp_err_t ads1115_set_gain(uint16_t gain) {
    uint16_t config;
    
    // Validate gain
    if ((gain & ~ADS111X_CFG_PGA_MASK) != 0 || 
        (gain != ADS111X_CFG_PGA_6_144V && 
         gain != ADS111X_CFG_PGA_4_096V && 
         gain != ADS111X_CFG_PGA_2_048V && 
         gain != ADS111X_CFG_PGA_1_024V && 
         gain != ADS111X_CFG_PGA_0_512V && 
         gain != ADS111X_CFG_PGA_0_256V)) {
        ESP_LOGE(TAG, "Invalid PGA gain setting: 0x%04x", gain);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Get current config
    esp_err_t ret = ads1115_get_config(&config);
    if (ret != ESP_OK) return ret;
    
    // Clear gain bits and set new gain
    config &= ~ADS111X_CFG_PGA_MASK;
    config |= gain;
    
    // Set the config
    ret = ads1115_set_config(config);
    if (ret == ESP_OK) {
        ads1115_state.current_pga = gain;
    }
    
    return ret;
}

// Set the data rate
esp_err_t ads1115_set_data_rate(uint16_t rate) {
    uint16_t config;
    
    // Validate rate
    if ((rate & ~ADS111X_CFG_DR_MASK) != 0) {
        ESP_LOGE(TAG, "Invalid data rate setting: 0x%04x", rate);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Get current config
    esp_err_t ret = ads1115_get_config(&config);
    if (ret != ESP_OK) return ret;
    
    // Clear data rate bits and set new rate
    config &= ~ADS111X_CFG_DR_MASK;
    config |= rate;
    
    // Set the config
    return ads1115_set_config(config);
}

// Configure ADS1115 to read from a specific single-ended channel (0-3)
esp_err_t ads1115_set_channel(uint8_t channel) {
    uint16_t config;
    uint16_t mux_value;
    
    if (channel > 3) {
        ESP_LOGE(TAG, "Invalid channel: %d (must be 0-3)", channel);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Get current config
    esp_err_t ret = ads1115_get_config(&config);
    if (ret != ESP_OK) return ret;
    
    // Determine MUX value based on channel
    switch (channel) {
        case 0: mux_value = ADS111X_CFG_MUX_SINGLE_0; break;
        case 1: mux_value = ADS111X_CFG_MUX_SINGLE_1; break;
        case 2: mux_value = ADS111X_CFG_MUX_SINGLE_2; break;
        case 3: mux_value = ADS111X_CFG_MUX_SINGLE_3; break;
        default: return ESP_ERR_INVALID_ARG;
    }
    
    // Clear MUX bits and set new MUX
    config &= ~ADS111X_CFG_MUX_MASK;
    config |= mux_value;
    
    // Set the config
    ret = ads1115_set_config(config);
    if (ret == ESP_OK) {
        ads1115_state.current_mux = (mux_value & ADS111X_CFG_MUX_MASK) >> ADS111X_CFG_MUX_SHIFT;
    }
    
    return ret;
}

// Start a conversion
esp_err_t ads1115_start_conversion(void) {
    uint16_t config;
    
    // Get current config
    esp_err_t ret = ads1115_get_config(&config);
    if (ret != ESP_OK) return ret;
    
    // Set OS bit to start conversion
    config |= ADS111X_CFG_OS_START;
    
    return ads1115_set_config(config);
}

esp_err_t ads1115_is_conversion_ready(bool *ready) {
    uint16_t config;
    
    esp_err_t ret = ads1115_get_config(&config);
    if (ret != ESP_OK) return ret;
    
    // OS bit is 1 when device is not performing a conversion
    *ready = (config & ADS111X_CFG_OS_MASK) == ADS111X_CFG_OS_IDLE;
    return ESP_OK;
}

esp_err_t ads1115_wait_conversion(void) {
    int timeout_ms = ADS1115_CONVERSION_TIMEOUT_MS;
    bool ready = false;
    esp_err_t ret;
    
    // Wait for conversion to complete with timeout
    while (timeout_ms > 0) {
        ret = ads1115_is_conversion_ready(&ready);
        if (ret != ESP_OK) return ret;
        
        if (ready) {
            return ESP_OK;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
        timeout_ms--;
    }
    
    ESP_LOGW(TAG, "Conversion timeout");
    return ESP_ERR_TIMEOUT;
}

// Read conversion result
esp_err_t ads1115_read_conversion(int16_t *result) {
    uint16_t value;
    
    esp_err_t ret = i2c_read_reg(ADS111X_REG_CONVERSION, &value);
    if (ret != ESP_OK) return ret;
    
    *result = (int16_t)value;
    return ESP_OK;
}

// conversion factor based on the current PGA setting
float ads1115_get_conversion_factor(void) {
    switch (ads1115_state.current_pga) {
        case ADS111X_CFG_PGA_6_144V: return ADS1115_MV_6_144V;
        case ADS111X_CFG_PGA_4_096V: return ADS1115_MV_4_096V;
        case ADS111X_CFG_PGA_2_048V: return ADS1115_MV_2_048V;
        case ADS111X_CFG_PGA_1_024V: return ADS1115_MV_1_024V;
        case ADS111X_CFG_PGA_0_512V: return ADS1115_MV_0_512V;
        case ADS111X_CFG_PGA_0_256V: return ADS1115_MV_0_256V;
        default: return ADS1115_MV_2_048V;  
    }
}

// High-level function to read voltage from a channel
esp_err_t ads1115_read_voltage(uint8_t channel, float *voltage) {
    esp_err_t ret;
    
    // Calculate current channel based on MUX value
    uint8_t current_channel;
    if (ads1115_state.current_mux >= 4) {
        current_channel = ads1115_state.current_mux - 4;  // Adjust for single-ended channels
    } else {
        // For differential inputs, we'll just set the new channel
        current_channel = 255;  // Invalid value to force channel change
    }
    
    // Set channel if different from current channel
    if (channel != current_channel) {
        ret = ads1115_set_channel(channel);
        if (ret != ESP_OK) return ret;
    }
    
    // If in single-shot mode, start a conversion
    if (!ads1115_state.is_continuous_mode) {
        ret = ads1115_start_conversion();
        if (ret != ESP_OK) return ret;
        
        // Wait for conversion to complete
        ret = ads1115_wait_conversion();
        if (ret != ESP_OK) return ret;
    } else {
        // In continuous mode, still wait a moment for a new reading
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    // Read the conversion result
    int16_t raw_value;
    ret = ads1115_read_conversion(&raw_value);
    if (ret != ESP_OK) return ret;
    
    // Convert to voltage based on the current PGA setting
    float factor = ads1115_get_conversion_factor();
    *voltage = raw_value * factor / 1000.0f;
    
    return ESP_OK;
}

// Simple API to read from AIN0-AIN3
float read_ain0(void) {
    float voltage = 0.0f;
    if (ads1115_read_voltage(0, &voltage) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read AIN0");
    }
    return voltage;
}

float read_ain1(void) {
    float voltage = 0.0f;
    if (ads1115_read_voltage(1, &voltage) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read AIN1");
    }
    return voltage;
}

float read_ain2(void) {
    float voltage = 0.0f;
    if (ads1115_read_voltage(2, &voltage) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read AIN2");
    }
    return voltage;
}

float read_ain3(void) {
    float voltage = 0.0f;
    if (ads1115_read_voltage(3, &voltage) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read AIN3");
    }
    return voltage;
}

// Dump the configuration register
esp_err_t ads1115_dump_config(void) {
    uint16_t config;
    esp_err_t ret = ads1115_get_config(&config);
    if (ret != ESP_OK) return ret;
    
    ESP_LOGI(TAG, "ADS1115 Config: 0x%04x", config);
    ESP_LOGI(TAG, "  OS:        %d", (config & ADS111X_CFG_OS_MASK) >> ADS111X_CFG_OS_SHIFT);
    ESP_LOGI(TAG, "  MUX:       %d", (config & ADS111X_CFG_MUX_MASK) >> ADS111X_CFG_MUX_SHIFT);
    ESP_LOGI(TAG, "  PGA:       %d", (config & ADS111X_CFG_PGA_MASK) >> ADS111X_CFG_PGA_SHIFT);
    ESP_LOGI(TAG, "  MODE:      %d", (config & ADS111X_CFG_MODE_MASK) >> ADS111X_CFG_MODE_SHIFT);
    ESP_LOGI(TAG, "  DR:        %d", (config & ADS111X_CFG_DR_MASK) >> ADS111X_CFG_DR_SHIFT);
    ESP_LOGI(TAG, "  COMP_MODE: %d", (config & ADS111X_CFG_CMODE_MASK) >> ADS111X_CFG_CMODE_SHIFT);
    ESP_LOGI(TAG, "  COMP_POL:  %d", (config & ADS111X_CFG_CPOL_MASK) >> ADS111X_CFG_CPOL_SHIFT);
    ESP_LOGI(TAG, "  COMP_LAT:  %d", (config & ADS111X_CFG_CLAT_MASK) >> ADS111X_CFG_CLAT_SHIFT);
    ESP_LOGI(TAG, "  COMP_QUE:  %d", (config & ADS111X_CFG_CQUE_MASK) >> ADS111X_CFG_CQUE_SHIFT);
    
    return ESP_OK;
}
