#ifndef __FUCTIONS_H
#define __FUNCTIONS_H

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "stm32f10x.h"
#include <stm32f10x_spi.h>
#include "nrf24.h"


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
nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length);


#endif // __FUNCTIONS_H
