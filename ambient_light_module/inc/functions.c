#include "functions.h"



void init_spi()
{

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

		    UART_SendStr("SPI is OK.\r\n");

}



void init_nrf24l01()
{

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

		    	// This is simple receiver with one RX pipe:
		    	//   - pipe#1 address: '0xE7 0x1C 0xE3'
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
		        // Configure RX PIPE#1
		        static const uint8_t nRF24_ADDR[] = { 0xE7, 0x1C, 0xE3 };
		        nRF24_SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for RX pipe #1
		        nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_OFF, 5); // Auto-ACK: disabled, payload length: 5 bytes
		        // Set operational mode (PRX == receiver)
		        nRF24_SetOperationalMode(nRF24_MODE_RX);
		        // Wake the transceiver
		        nRF24_SetPowerMode(nRF24_PWR_UP);
		        // Put the transceiver to the RX mode
		        nRF24_CE_H();
}




void SetSysClockTo72(void)
{
    ErrorStatus HSEStartUpStatus;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig( RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
        //FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

        /* Flash 2 wait state */
        //FLASH_SetLatency( FLASH_Latency_2);

        /* HCLK = SYSCLK */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(0x00010000, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd( ENABLE);

        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock configuration.
     User can add here some code to deal with this error */

        /* Go to infinite loop */
        while (1)
        {
        }
    }
}

