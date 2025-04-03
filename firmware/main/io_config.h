// Global IO pin configuration

#pragma once

#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_20

#define BOOST_EN GPIO_NUM_21

extern gpio_config_t io_config;
