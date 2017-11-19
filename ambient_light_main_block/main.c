#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>

#include "uart.h"
#include "delay.h"
#include "nrf24.h"

//#include <disp1color.h>
//#include <font.h>


uint32_t i,j,k;

// Buffer to store a payload of maximum width
uint8_t nRF24_payload[32];

// Pipe number
nRF24_RXResult pipe;

// Length of received payload
uint8_t payload_length;

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

// Function to transmit data packet
// input:
//   pBuf - pointer to the buffer with data to transmit
//   length - length of the data buffer in bytes
// return: one of nRF24_TX_xx values
nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length) {
	volatile uint32_t wait = nRF24_WAIT_TIMEOUT;
	uint8_t status;

	// Deassert the CE pin (in case if it still high)
	nRF24_CE_L();

	// Transfer a data from the specified buffer to the TX FIFO
	nRF24_WritePayload(pBuf, length);

	// Start a transmission by asserting CE pin (must be held at least 10us)
	nRF24_CE_H();

	// Poll the transceiver status register until one of the following flags will be set:
	//   TX_DS  - means the packet has been transmitted
	//   MAX_RT - means the maximum number of TX retransmits happened
	// note: this solution is far from perfect, better to use IRQ instead of polling the status
	do {
		status = nRF24_GetStatus();
		if (status & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT)) {
			break;
		}
	} while (wait--);

	// Deassert the CE pin (Standby-II --> Standby-I)
	nRF24_CE_L();

	if (!wait) {
		// Timeout
		return nRF24_TX_TIMEOUT;
	}

	// Check the flags in STATUS register
	UART_SendStr("[");
	UART_SendHex8(status);
	UART_SendStr("] ");

	// Clear pending IRQ flags
    nRF24_ClearIRQFlags();

	if (status & nRF24_FLAG_MAX_RT) {
		// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
		return nRF24_TX_MAXRT;
	}

	if (status & nRF24_FLAG_TX_DS) {
		// Successful transmission
		return nRF24_TX_SUCCESS;
	}

	// Some banana happens, a payload remains in the TX FIFO, flush it
	nRF24_FlushTX();

	return nRF24_TX_ERROR;
}




int main(void)
{
	UART_Init(115200);
	    UART_SendStr("\r\nSTM32F103RET6 is online.\r\n");


	    GPIO_InitTypeDef PORT;
	    SPI_InitTypeDef SPI;


	    // Enable SPI2 peripheral
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

	    // Enable SPI2 GPIO peripheral (PORTB)
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);


	    // Configure nRF24 IRQ pin
		PORT.GPIO_Mode  = GPIO_Mode_Out_PP;
		PORT.GPIO_Speed = GPIO_Speed_2MHz;
		PORT.GPIO_Pin   = nRF24_IRQ_PIN;
		GPIO_Init(nRF24_IRQ_PORT, &PORT);

		// Configure SPI pins (SPI2)
	    PORT.GPIO_Mode  = GPIO_Mode_AF_PP;
	    PORT.GPIO_Speed = GPIO_Speed_50MHz;
	    PORT.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	    GPIO_Init(GPIOB, &PORT);


	    // Initialize SPI2
	    SPI.SPI_Mode = SPI_Mode_Master;
	    SPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	    SPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	    SPI.SPI_CPOL = SPI_CPOL_Low;
	    SPI.SPI_CPHA = SPI_CPHA_1Edge;
	    SPI.SPI_CRCPolynomial = 7;
	    SPI.SPI_DataSize = SPI_DataSize_8b;
	    SPI.SPI_FirstBit = SPI_FirstBit_MSB;
	    SPI.SPI_NSS = SPI_NSS_Soft;
	    SPI_Init(nRF24_SPI_PORT, &SPI);
	    SPI_NSSInternalSoftwareConfig(nRF24_SPI_PORT, SPI_NSSInternalSoft_Set);
	    SPI_Cmd(nRF24_SPI_PORT, ENABLE);


	    // Initialize delay
	    Delay_Init();


	    // Initialize the nRF24L01 GPIO pins
	    nRF24_GPIO_Init();

	    // RX/TX disabled
	    nRF24_CE_L();

	    // Configure the nRF24L01+
	    UART_SendStr("nRF24L01+ check: ");
	    if (!nRF24_Check()) {
	    	UART_SendStr("FAIL\r\n");
	    	while (1);
	    }
		UART_SendStr("OK\r\n");

	    // Initialize the nRF24L01 to its default state
	    nRF24_Init();


	    // This is simple transmitter (to one logic address):
	    	//   - TX address: '0xE7 0x1C 0xE3'
	    	//   - payload: 5 bytes
	    	//   - RF channel: 115 (2515MHz)
	    	//   - data rate: 250kbps (minimum possible, to increase reception reliability)
	    	//   - CRC scheme: 2 byte

	        // The transmitter sends a 5-byte packets to the address '0xE7 0x1C 0xE3' without Auto-ACK (ShockBurst disabled)

	        // Disable ShockBurst for all RX pipes
	        nRF24_DisableAA(0xFF);

	        // Set RF channel
	        nRF24_SetRFChannel(115);

	        // Set data rate
	        nRF24_SetDataRate(nRF24_DR_250kbps);

	        // Set CRC scheme
	        nRF24_SetCRCScheme(nRF24_CRC_2byte);

	        // Set address width, its common for all pipes (RX and TX)
	        nRF24_SetAddrWidth(3);

	        // Configure TX PIPE
	        static const uint8_t nRF24_ADDR[] = { 0xE7, 0x1C, 0xE3 };
	        nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address

	        // Set TX power (maximum)
	        nRF24_SetTXPower(nRF24_TXPWR_0dBm);

	        // Set operational mode (PTX == transmitter)
	        nRF24_SetOperationalMode(nRF24_MODE_TX);

	        // Clear any pending IRQ flags
	        nRF24_ClearIRQFlags();

	        // Wake the transceiver
	        nRF24_SetPowerMode(nRF24_PWR_UP);


	        ////////////////////
	       //disp1color_Init();
	      // disp1color_SetBrightness(255);

	        // ����� �� ����������
	        //disp1color_DrawRectangle(5, 5, 122, 58);
	        // ����������� (� ������ ���������������� ������ �� �������)
	        //disp1color_printf(10, 8, FONTID_10X16F, "�����������\n\r� ���������\n\r%d", 2016);
	        //disp1color_UpdateFromBuff();
	      //  Delay_ms(4000);

	    	//////////////////////////



	        // The main loop
	        j = 0;
	        payload_length = 5;
	        while (1) {
	        	// Prepare data packet
	        	for (i = 0; i < payload_length; i++) {
	        		nRF24_payload[i] = j++;
	        		if (j > 0x000000FF) j = 0;
	        	}

	        	// Print a payload
	        	UART_SendStr("PAYLOAD:>");
	        	UART_SendBufHex((char *)nRF24_payload, payload_length);
	        	UART_SendStr("< ... TX: ");

	        	// Transmit a packet
	        	tx_res = nRF24_TransmitPacket(nRF24_payload, payload_length);
	        	switch (tx_res) {
	    			case nRF24_TX_SUCCESS:
	    				UART_SendStr("OK");
	    				break;
	    			case nRF24_TX_TIMEOUT:
	    				UART_SendStr("TIMEOUT");
	    				break;
	    			case nRF24_TX_MAXRT:
	    				UART_SendStr("MAX RETRANSMIT");
	    				break;
	    			default:
	    				UART_SendStr("ERROR");
	    				break;
	    		}
	        	UART_SendStr("\r\n");

	        	// Wait ~0.5s
	        	Delay_ms(2000);
	        }

}
