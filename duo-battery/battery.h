#pragma once
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ADC_CHANNEL '1'
#define ADC_MAX_VALUE 4096 // Adjustment for error in resistor values
#define R5 20.0            // Resistor value in kOhm
#define R6 68.0            // Resistor value in kOhm

/// @brief Initialize ADC
/// @return ADC file descriptor, -1 if failed
int adc_init();

/// @brief Read voltage from ADC
/// @param fd ADC file descriptor
/// @return float voltage value considering voltage divider
float read_voltage(int fd);