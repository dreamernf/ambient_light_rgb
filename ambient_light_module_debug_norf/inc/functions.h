#ifndef __FUCTIONS_H
#define __FUNCTIONS_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "stm32f10x.h"
#include <stm32f10x_spi.h>
#include <stm32f10x_tim.h>
#include "stm32_ub_dig_in.h"
#include "nrf24.h"

#define  COUNT_STEPS_BRIGHT    15

#define  MIN_ADC  0
#define  MAX_ADC  4053


struct RGB_COLOR_TYPE
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};



// Timeout counter (depends on the CPU speed)
// Used for not stuck waiting for IRQ
#define nRF24_WAIT_TIMEOUT         (uint32_t)0x000FFFFF



void init_spi();
void init_adc();
void init_nrf24l01();
void SetSysClockTo72();
uint8_t set_brightness_slave(int8_t  bright_step, int8_t bright_nrfl);


#endif // __FUNCTIONS_H
