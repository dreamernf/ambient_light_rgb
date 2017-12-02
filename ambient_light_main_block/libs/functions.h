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


#define  RED_CH    TIM3->CCR1
#define  GREEN_CH  TIM3->CCR3
#define  BLUE_CH   TIM3->CCR2



#define  PERIOD 255

#define  BRIGHTNESS_100     255
#define  BRIGHTNESS_75      192
#define  BRIGHTNESS_50      127
#define  BRIGHTNESS_25      64
#define  BRIGHTNESS_10      32
#define  BRIGHTNESS_5       16
#define  BRIGHTNESS_0       0
#define  LIGHT_OFF          0

#define  ADC_100            2400
#define  ADC_0			    902
#define  ADC_5			    1116
#define  ADC_10				1330
#define  ADC_25				1544
#define  ADC_50				1758
#define  ADC_75				1972



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
void init_pwm();
void init_adc();
nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length);
void set_color(struct RGB_COLOR_TYPE color, unsigned char brightness);
void SetSysClockTo72();
uint8_t set_brightness(uint16_t voltage);


#endif // __FUNCTIONS_H
