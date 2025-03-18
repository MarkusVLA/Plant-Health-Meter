// Register addresses and bit definitions for the BME280 sensor based on datasheet

#pragma once

/* BME280 I2C Addresses */
#define BME280_I2C_ADDR_PRIMARY      0x76
#define BME280_I2C_ADDR_SECONDARY    0x77

/* Chip ID */
#define BME280_CHIP_ID               0x60

/* Register Addresses */
#define BME280_REG_CHIP_ID           0xD0
#define BME280_REG_RESET             0xE0
#define BME280_REG_CTRL_HUM          0xF2
#define BME280_REG_STATUS            0xF3
#define BME280_REG_CTRL_MEAS         0xF4
#define BME280_REG_CONFIG            0xF5
#define BME280_REG_PRESS_MSB         0xF7
#define BME280_REG_PRESS_LSB         0xF8
#define BME280_REG_PRESS_XLSB        0xF9
#define BME280_REG_TEMP_MSB          0xFA
#define BME280_REG_TEMP_LSB          0xFB
#define BME280_REG_TEMP_XLSB         0xFC
#define BME280_REG_HUM_MSB           0xFD
#define BME280_REG_HUM_LSB           0xFE

/* Calibration Data Registers */
#define BME280_REG_CALIB00           0x88
#define BME280_REG_CALIB25           0xA1
#define BME280_REG_CALIB26           0xE1
#define BME280_REG_CALIB41           0xF0

/* Soft Reset Command */
#define BME280_RESET_VALUE           0xB6

/* Status Register Bits */
#define BME280_STATUS_MEASURING_BIT  3
#define BME280_STATUS_IM_UPDATE_BIT  0

/* Mode Definitions */
#define BME280_MODE_SLEEP            0x00
#define BME280_MODE_FORCED           0x01
#define BME280_MODE_NORMAL           0x03
#define BME280_MODE_MASK             0x03

/* Oversampling Definitions for Humidity (ctrl_hum register) */
#define BME280_OSRS_H_NONE           0x00 /* Skipped (output set to 0x8000) */
#define BME280_OSRS_H_1X             0x01 /* Oversampling x1 */
#define BME280_OSRS_H_2X             0x02 /* Oversampling x2 */
#define BME280_OSRS_H_4X             0x03 /* Oversampling x4 */
#define BME280_OSRS_H_8X             0x04 /* Oversampling x8 */
#define BME280_OSRS_H_16X            0x05 /* Oversampling x16 */
#define BME280_OSRS_H_MASK           0x07

/* Oversampling Definitions for Temperature (ctrl_meas register) */
#define BME280_OSRS_T_NONE           0x00 /* Skipped (output set to 0x80000) */
#define BME280_OSRS_T_1X             0x20 /* Oversampling x1 */
#define BME280_OSRS_T_2X             0x40 /* Oversampling x2 */
#define BME280_OSRS_T_4X             0x60 /* Oversampling x4 */
#define BME280_OSRS_T_8X             0x80 /* Oversampling x8 */
#define BME280_OSRS_T_16X            0xA0 /* Oversampling x16 */
#define BME280_OSRS_T_MASK           0xE0
#define BME280_OSRS_T_POS            5

/* Oversampling Definitions for Pressure (ctrl_meas register) */
#define BME280_OSRS_P_NONE           0x00 /* Skipped (output set to 0x80000) */
#define BME280_OSRS_P_1X             0x04 /* Oversampling x1 */
#define BME280_OSRS_P_2X             0x08 /* Oversampling x2 */
#define BME280_OSRS_P_4X             0x0C /* Oversampling x4 */
#define BME280_OSRS_P_8X             0x10 /* Oversampling x8 */
#define BME280_OSRS_P_16X            0x14 /* Oversampling x16 */
#define BME280_OSRS_P_MASK           0x1C
#define BME280_OSRS_P_POS            2

/* Filter Coefficient Definitions (config register) */
#define BME280_FILTER_OFF            0x00
#define BME280_FILTER_COEF_2         0x04
#define BME280_FILTER_COEF_4         0x08
#define BME280_FILTER_COEF_8         0x0C
#define BME280_FILTER_COEF_16        0x10
#define BME280_FILTER_MASK           0x1C
#define BME280_FILTER_POS            2

/* Standby Time Definitions (config register) */
#define BME280_STANDBY_0_5_MS        0x00
#define BME280_STANDBY_62_5_MS       0x20
#define BME280_STANDBY_125_MS        0x40
#define BME280_STANDBY_250_MS        0x60
#define BME280_STANDBY_500_MS        0x80
#define BME280_STANDBY_1000_MS       0xA0
#define BME280_STANDBY_10_MS         0xC0
#define BME280_STANDBY_20_MS         0xE0
#define BME280_STANDBY_MASK          0xE0
#define BME280_STANDBY_POS           5

/* SPI 3-Wire Interface (config register) */
#define BME280_SPI3W_EN_BIT          0

