#ifndef __FUCTIONS_H
#define __FUNCTIONS_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "stm32f10x.h"
#include <stm32f10x_spi.h>
#include <stm32f10x_tim.h>
#include "nrf24.h"


struct RGB_COLOR_TYPE
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};





//#define  STEP_BRIGHT		(MAX_ADC-MIN_ADC)/count_pwm_steps



// Helpers for transmit mode demo

// Timeout counter (depends on the CPU speed)
// Used for not stuck waiting for IRQ
#define nRF24_WAIT_TIMEOUT         (uint32_t)0x000FFFFF



void init_spi();
void init_nrf24l01();
void SetSysClockTo72();


#endif // __FUNCTIONS_H
