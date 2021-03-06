#include "functions.h"


void init_spi()
{

	 	 	GPIO_InitTypeDef PORT;
		    SPI_InitTypeDef SPI;



		    // Enable SPI2 peripheral
		    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

		    // Enable SPI2 GPIO peripheral (PORTB)
		    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);


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


}


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


void init_nrf24l01()
{

				// Initialize the nRF24L01 GPIO pins
				nRF24_GPIO_Init();
				// RX/TX disabled
				nRF24_CE_L();

				// Configure the nRF24L01+

				if (!nRF24_Check()) {

					while (1);
				}


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
}


void init_adc(){

	//ADC
	    ADC_InitTypeDef ADC_InitStructure;
	    GPIO_InitTypeDef  GPIO_InitStructure;
	    // input of ADC (it doesn't seem to be needed, as default GPIO state is floating input)
	    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 ;        // that's ADC1 (PA1 on STM32)
	    GPIO_Init(GPIOA, &GPIO_InitStructure);

	    //clock for ADC (max 14MHz --> 72/6=12MHz)
	    RCC_ADCCLKConfig (RCC_PCLK2_Div6);
	    // enable ADC system clock
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	    // define ADC config
	    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // we work in continuous sampling mode
	    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	    ADC_InitStructure.ADC_NbrOfChannel = 1;

	    ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_28Cycles5); // define regular conversion config
	    ADC_Init ( ADC1, &ADC_InitStructure);   //set config of ADC1

	    // enable ADC
	    ADC_Cmd (ADC1,ENABLE);  //enable ADC1

	    //  ADC calibration (optional, but recommended at power on)
	    ADC_ResetCalibration(ADC1); // Reset previous calibration
	    while(ADC_GetResetCalibrationStatus(ADC1));
	    ADC_StartCalibration(ADC1); // Start new calibration (ADC must be off at that time)
	    while(ADC_GetCalibrationStatus(ADC1));

	    // start conversion
	    ADC_Cmd (ADC1,ENABLE);  //enable ADC1
	    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // start conversion (will be endless as
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

uint8_t set_brightness(int16_t voltage)
{
	uint8_t  br_tmp = 0;

	br_tmp = (uint8_t)((voltage - MIN_ADC)/((MAX_ADC-MIN_ADC)/count_pwm_steps));

	if (voltage>=MAX_ADC) {br_tmp = count_pwm_steps-1;};

	if (br_tmp<=0 )  {br_tmp = 0;};

	if (voltage <= MIN_ADC)  {br_tmp = 0;};

	if (br_tmp>=count_pwm_steps)  {br_tmp = count_pwm_steps-1;};

	return br_tmp;

}



