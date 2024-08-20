#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ADC_CHANNEL '1'
#define R5 20.0 // Resistor value in kOhm
#define R6 68.0 // Resistor value in kOhm

int adc_init();

float read_voltage(int fd);