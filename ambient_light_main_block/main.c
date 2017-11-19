#include "uart.h"
#include "delay.h"
#include "nrf24.h"
#include "functions.h"


		uint32_t i,j,k;

		// Buffer to store a payload of maximum width
		uint8_t nRF24_payload[32];

		// Pipe number
		nRF24_RXResult pipe;

		// Length of received payload
		uint8_t payload_length;



int main(void)
{

	        UART_Init(115200);

			init_spi();

			Delay_Init();

			init_nrf24l01();

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
	        	Delay_ms(500);
	        }

}
