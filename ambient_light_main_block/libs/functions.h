#ifndef __FUCTIONS_H
#define __FUNCTIONS_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "stm32f10x.h"
#include <stm32f10x_spi.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_adc.h>
#include "nrf24.h"

struct RGB_COLOR_TYPE
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};


#define  PERIOD   256
#define  PERIOD_T 1000

#define  count_pwm_steps    128

#define  MIN_ADC			400
#define  MAX_ADC			2500
//#define  STEP_BRIGHT		(MAX_ADC-MIN_ADC)/count_pwm_steps



// Helpers for transmit mode demo

// Timeout counter (depends on the CPU speed)
// Used for not stuck waiting for IRQ
#define nRF24_WAIT_TIMEOUT         (uint32_t)0x000FFFFF

// Result of packet transmission
typedef enum {
	nRF24_TX_ERROR  = (uint8_t)0x00, // Unknown error
	nRF24_TX_SUCCESS,                // Packet has been transmitted successfully
	nRF24_TX_TIMEOUT,                // It was timeout during packet transmit
	nRF24_TX_MAXRT                   // Transmit failed with maximum auto retransmit count
} nRF24_TXResult;

nRF24_TXResult tx_res;


void init_spi();
void init_nrf24l01();
void init_adc();
nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length);
void SetSysClockTo72();
uint8_t set_brightness(int16_t voltage);


#endif // __FUNCTIONS_H
