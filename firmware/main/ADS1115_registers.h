// Register mappings and command definitions for Texas Instruments ADS1115 ADC
#pragma once

// ADS111x I2C addresses based on ADDR pin connection 
#define ADS111X_I2C_ADDR_GND      0x48 // ADDR pin connected to GND (1001000b)
#define ADS111X_I2C_ADDR_VDD      0x49 // ADDR pin connected to VDD (1001001b)
#define ADS111X_I2C_ADDR_SDA      0x4A // ADDR pin connected to SDA (1001010b)
#define ADS111X_I2C_ADDR_SCL      0x4B // ADDR pin connected to SCL (1001011b)

#define ADS111X_REG_CONVERSION    0x00 // Conversion register
#define ADS111X_REG_CONFIG        0x01 // Configuration register
#define ADS111X_REG_LO_THRESH     0x02 // Low threshold register
#define ADS111X_REG_HI_THRESH     0x03 // High threshold register

// Operational status / Single-shot conversion start
#define ADS111X_CFG_OS_MASK       0x8000
#define ADS111X_CFG_OS_SHIFT      15
#define ADS111X_CFG_OS_IDLE       0x8000 // When reading: 1 = device idle
#define ADS111X_CFG_OS_BUSY       0x0000 // When reading: 0 = device performing conversion
#define ADS111X_CFG_OS_START      0x8000 // When writing: 1 = start single conversion (in power-down mode)

// Input multiplexer configuration (ADS1115 only)
#define ADS111X_CFG_MUX_MASK      0x7000
#define ADS111X_CFG_MUX_SHIFT     12
#define ADS111X_CFG_MUX_DIFF_0_1  0x0000 // Default: AINP = AIN0, AINN = AIN1
#define ADS111X_CFG_MUX_DIFF_0_3  0x1000 // AINP = AIN0, AINN = AIN3
#define ADS111X_CFG_MUX_DIFF_1_3  0x2000 // AINP = AIN1, AINN = AIN3
#define ADS111X_CFG_MUX_DIFF_2_3  0x3000 // AINP = AIN2, AINN = AIN3
#define ADS111X_CFG_MUX_SINGLE_0  0x4000 // AINP = AIN0, AINN = GND
#define ADS111X_CFG_MUX_SINGLE_1  0x5000 // AINP = AIN1, AINN = GND
#define ADS111X_CFG_MUX_SINGLE_2  0x6000 // AINP = AIN2, AINN = GND
#define ADS111X_CFG_MUX_SINGLE_3  0x7000 // AINP = AIN3, AINN = GND

// Programmable gain amplifier configuration (ADS1114 and ADS1115 only)
#define ADS111X_CFG_PGA_MASK      0x0E00
#define ADS111X_CFG_PGA_SHIFT     9
#define ADS111X_CFG_PGA_6_144V    0x0000 // FSR = ±6.144V
#define ADS111X_CFG_PGA_4_096V    0x0200 // FSR = ±4.096V
#define ADS111X_CFG_PGA_2_048V    0x0400 // FSR = ±2.048V (default)
#define ADS111X_CFG_PGA_1_024V    0x0600 // FSR = ±1.024V
#define ADS111X_CFG_PGA_0_512V    0x0800 // FSR = ±0.512V
#define ADS111X_CFG_PGA_0_256V    0x0A00 // FSR = ±0.256V

// Device operating mode
#define ADS111X_CFG_MODE_MASK     0x0100
#define ADS111X_CFG_MODE_SHIFT    8
#define ADS111X_CFG_MODE_CONTIN   0x0000 // Continuous conversion mode
#define ADS111X_CFG_MODE_SINGLE   0x0100 // Single-shot conversion mode (default)

// Data rate
#define ADS111X_CFG_DR_MASK       0x00E0
#define ADS111X_CFG_DR_SHIFT      5
#define ADS111X_CFG_DR_8SPS       0x0000 // 8 samples per second
#define ADS111X_CFG_DR_16SPS      0x0020 // 16 samples per second
#define ADS111X_CFG_DR_32SPS      0x0040 // 32 samples per second
#define ADS111X_CFG_DR_64SPS      0x0060 // 64 samples per second
#define ADS111X_CFG_DR_128SPS     0x0080 // 128 samples per second (default)
#define ADS111X_CFG_DR_250SPS     0x00A0 // 250 samples per second
#define ADS111X_CFG_DR_475SPS     0x00C0 // 475 samples per second
#define ADS111X_CFG_DR_860SPS     0x00E0 // 860 samples per second

// Comparator mode (ADS1114 and ADS1115 only)
#define ADS111X_CFG_CMODE_MASK    0x0010
#define ADS111X_CFG_CMODE_SHIFT   4
#define ADS111X_CFG_CMODE_TRAD    0x0000 // Traditional comparator (default)
#define ADS111X_CFG_CMODE_WINDOW  0x0010 // Window comparator

// Comparator polarity (ADS1114 and ADS1115 only)
#define ADS111X_CFG_CPOL_MASK     0x0008
#define ADS111X_CFG_CPOL_SHIFT    3
#define ADS111X_CFG_CPOL_ACTVLOW  0x0000 // ALERT/RDY pin is active low (default)
#define ADS111X_CFG_CPOL_ACTVHIGH 0x0008 // ALERT/RDY pin is active high

// Latching comparator (ADS1114 and ADS1115 only)
#define ADS111X_CFG_CLAT_MASK     0x0004
#define ADS111X_CFG_CLAT_SHIFT    2
#define ADS111X_CFG_CLAT_NONLAT   0x0000 // Non-latching comparator (default)
#define ADS111X_CFG_CLAT_LATCH    0x0004 // Latching comparator

// Comparator queue and disable (ADS1114 and ADS1115 only)
#define ADS111X_CFG_CQUE_MASK     0x0003
#define ADS111X_CFG_CQUE_SHIFT    0
#define ADS111X_CFG_CQUE_1CONV    0x0000 // Assert after one conversion
#define ADS111X_CFG_CQUE_2CONV    0x0001 // Assert after two conversions
#define ADS111X_CFG_CQUE_4CONV    0x0002 // Assert after four conversions
#define ADS111X_CFG_CQUE_DISABLE  0x0003 // Disable comparator and set ALERT/RDY pin to high-impedance (default)

// Default config
#define ADS111X_CONFIG_DEFAULT    0x8583 // Default configuration
#define ADS111X_HI_THRESH_RDY_MSB 0x8000 // MSB=1 for conversion-ready mode
#define ADS111X_LO_THRESH_RDY_MSB 0x0000 // MSB=0 for conversion-ready mode
#define ADS111X_CONV_MASK         0xFFFF // Full 16-bit result mask

// Helper macros
#define ADS111X_CFG_SET_OS(reg, value)     (((reg) & ~ADS111X_CFG_OS_MASK) | ((value) << ADS111X_CFG_OS_SHIFT))
#define ADS111X_CFG_SET_MUX(reg, value)    (((reg) & ~ADS111X_CFG_MUX_MASK) | ((value) << ADS111X_CFG_MUX_SHIFT))
#define ADS111X_CFG_SET_PGA(reg, value)    (((reg) & ~ADS111X_CFG_PGA_MASK) | ((value) << ADS111X_CFG_PGA_SHIFT))
#define ADS111X_CFG_SET_MODE(reg, value)   (((reg) & ~ADS111X_CFG_MODE_MASK) | ((value) << ADS111X_CFG_MODE_SHIFT))
#define ADS111X_CFG_SET_DR(reg, value)     (((reg) & ~ADS111X_CFG_DR_MASK) | ((value) << ADS111X_CFG_DR_SHIFT))
#define ADS111X_CFG_SET_CMODE(reg, value)  (((reg) & ~ADS111X_CFG_CMODE_MASK) | ((value) << ADS111X_CFG_CMODE_SHIFT))
#define ADS111X_CFG_SET_CPOL(reg, value)   (((reg) & ~ADS111X_CFG_CPOL_MASK) | ((value) << ADS111X_CFG_CPOL_SHIFT))
#define ADS111X_CFG_SET_CLAT(reg, value)   (((reg) & ~ADS111X_CFG_CLAT_MASK) | ((value) << ADS111X_CFG_CLAT_SHIFT))
#define ADS111X_CFG_SET_CQUE(reg, value)   (((reg) & ~ADS111X_CFG_CQUE_MASK) | ((value) << ADS111X_CFG_CQUE_SHIFT))

