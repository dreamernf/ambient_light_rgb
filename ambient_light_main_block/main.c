#include "uart.h"
#include "delay.h"
#include "nrf24.h"
#include "functions.h"
#include "stm32_ub_led.h"
#include "stm32_ub_button.h"
#include "stm32f10x_flash.h"
#include <stdio.h>



struct RGB_COLOR_TYPE Red =          {255, 0, 0};
struct RGB_COLOR_TYPE Orange =       {255, 128, 0};
struct RGB_COLOR_TYPE Yellow =       {252, 255, 0};
struct RGB_COLOR_TYPE Green =        {0, 255, 0};
struct RGB_COLOR_TYPE GreenLime =    {191,255,  0};
struct RGB_COLOR_TYPE LightGreen =   {128,255,  0};
struct RGB_COLOR_TYPE Blue =         {0, 0, 255};
struct RGB_COLOR_TYPE NavyBlue =     {0, 0, 255};
struct RGB_COLOR_TYPE Violet =       {200, 0, 255};
struct RGB_COLOR_TYPE White =        {255, 255, 255};
struct RGB_COLOR_TYPE Cyan =         {  0,255,255};
struct RGB_COLOR_TYPE LightBlue =    {  0,128,255};
struct RGB_COLOR_TYPE Purple =       {128,  0,255};
struct RGB_COLOR_TYPE Black =        {0, 0, 0};


const int16_t RGB_PWM[count_pwm_steps] = {
		0 , 0 , 0 , 0 , 0 , 1 , 1 , 1 , 2 , 2 , 3 , 3 , 4 , 4 , 5 , 6,
		6 , 7 , 8 , 9 , 9 , 10 , 11 , 12 , 13 , 14 , 15 , 16 , 17 , 18 , 19 , 21,
		22 , 23 , 24 , 25 , 27 , 28 , 30 , 31 , 32 , 34 , 35 , 37 , 38 , 40 , 42 , 43,
		45 , 46 , 48 , 50 , 52 , 53 , 55 , 57 , 59 , 61 , 63 , 65 , 67 , 69 , 71 , 73,
		75 , 77 , 79 , 81 , 83 , 86 , 88 , 90 , 92 , 95 , 97 , 99 , 102 , 104 , 106 , 109,
		111 , 114 , 116 , 119 , 122 , 124 , 127 , 129 , 132 , 135 , 137 , 140 , 143 , 146 , 149 , 151,
		154 , 157 , 160 , 163 , 166 , 169 , 172 , 175 , 178 , 181 , 184 , 187 , 190 , 194 , 197 , 200,
		203 , 207 , 210 , 213 , 216 , 220 , 223 , 227 , 230 , 233 , 237 , 240 , 244 , 247 , 251 , 255
		};




		uint32_t i,j,k, avg_adc;

		uint8_t bright_rgb = 0;

		uint8_t m = 0;

		uint8_t  number_color = 1;


		uint16_t adc_value = 0;

        char buffer[30];

		// Buffer to store a payload of maximum width
		uint8_t nRF24_payload[32];

		// Pipe number
		nRF24_RXResult pipe;

		// Length of received payload
		uint8_t payload_length = 5;

		// ====================================================
		// FLASH Settings struct
		// ====================================================
		#define MY_FLASH_PAGE_ADDR 0x800FC00

		typedef struct
		  {
			uint8_t  NumberColorF;    // 1 byte
		    unsigned char F1;  // 1 byte
		    unsigned char F2;    // 1 byte
		    unsigned char F3;    // 1 byte

		                            // 8 byte = 2  32-bits words.  It's - OK
		                            // !!! Full size (bytes) must be a multiple of 4 !!!
		  } tpSettings;

		tpSettings settings;

		#define SETTINGS_WORDS sizeof(settings)/4

		void FLASH_Init(void) {
		    /* Next commands may be used in SysClock initialization function
		       In this case using of FLASH_Init is not obligatorily */
		    /* Enable Prefetch Buffer */
		    FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);
		    /* Flash 2 wait state */
		    FLASH_SetLatency( FLASH_Latency_2);
		}

		void FLASH_ReadSettings(void) {
		    //Read settings
		    uint32_t *source_addr = (uint32_t *)MY_FLASH_PAGE_ADDR;
		    uint32_t *dest_addr = (void *)&settings;
		    for (uint16_t i=0; i<SETTINGS_WORDS; i++) {
		        *dest_addr = *(__IO uint32_t*)source_addr;
		        source_addr++;
		        dest_addr++;
		    }
		}

		void FLASH_WriteSettings(void) {
		    FLASH_Unlock();
		    FLASH_ErasePage(MY_FLASH_PAGE_ADDR);

		    // Write settings
		    uint32_t *source_addr = (void *)&settings;
		    uint32_t *dest_addr = (uint32_t *) MY_FLASH_PAGE_ADDR;
		    for (uint16_t i=0; i<SETTINGS_WORDS; i++) {
		        FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
		        source_addr++;
		        dest_addr++;
		    }

		    FLASH_Lock();
		}
		// ====================================================




int main(void)
{

	        SetSysClockTo72();

	        FLASH_Init();

#ifdef DEBUG
	        UART_Init(115200);
#endif

			init_spi();

			Delay_Init();

			init_nrf24l01();

			UB_Led_Init();

			UB_Button_Init();

			init_pwm();

			init_adc();

			UB_Led_On(LED_DEBUG);

			set_color(Green, RGB_PWM[count_pwm_steps-1]);
			Delay_ms(500);


			UB_Led_On(LED_BO);

        	UB_Led_On(BUZZER);
        	Delay_ms(100);
        	UB_Led_Off(BUZZER);

        	FLASH_ReadSettings();

        	number_color = settings.NumberColorF;


	        // The main loop
	        j = 0;
	        while (1) {

	        	// Wait ~0.5s
	        	//UB_Led_Toggle(LED_DEBUG);
	        	//UB_Led_Toggle(BUZZER);

	        	avg_adc = 0;
	        	for (m=1;m<=50;m++)
	        	{
	        		avg_adc = avg_adc + ADC_GetConversionValue(ADC1);
	        		Delay_ms(1);
	        	}

	        	adc_value = avg_adc/50;

	        	bright_rgb = set_brightness(adc_value);

#ifdef DEBUG
       	        sprintf(buffer, "V = %d  STEP NUM = %d\r\n", adc_value,bright_rgb);
       	        UART_SendStr(buffer);
	        	Delay_ms(500);
#endif


        		switch (number_color) {
        		case 1: set_color(Red,RGB_PWM[bright_rgb]);break;
        		case 2: set_color(Green,RGB_PWM[bright_rgb]); break;
        		case 3: set_color(Blue,RGB_PWM[bright_rgb]); break;
        		case 4: set_color(Orange,RGB_PWM[bright_rgb]); break;
        		case 5: set_color(Yellow,RGB_PWM[bright_rgb]); break;
        		case 6: set_color(GreenLime,RGB_PWM[bright_rgb]); break;
        		case 7: set_color(LightGreen,RGB_PWM[bright_rgb]); break;
        		case 8: set_color(NavyBlue,RGB_PWM[bright_rgb]); break;
        		case 9: set_color(Violet,RGB_PWM[bright_rgb]); break;
        		case 10: set_color(White,RGB_PWM[bright_rgb]); break;
        		case 11: set_color(Cyan,RGB_PWM[bright_rgb]); break;
        		case 12: set_color(LightGreen,RGB_PWM[bright_rgb]); break;
        		case 13: set_color(Purple,RGB_PWM[bright_rgb]); break;
        		case 14: set_color(Black,RGB_PWM[bright_rgb]); break;
        		}

	        	nRF24_payload[0] = 	number_color;
	        	nRF24_payload[1] =  bright_rgb;
	        	nRF24_payload[2] = 	2;
	        	nRF24_payload[3] = 	3;
	        	nRF24_payload[4] = 	4;
	        	nRF24_payload[5] = 	5;


#ifdef DEBUG
	        	// Print a payload
	        	UART_SendStr("PAYLOAD:>");
	        	UART_SendBufHex((char *)nRF24_payload, payload_length);
	        	UART_SendStr("< ... TX: ");
#endif

	        	// Transmit a packet
	        	tx_res = nRF24_TransmitPacket(nRF24_payload, payload_length);
#ifdef DEBUG
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
#endif


	        	if (UB_Button_OnClick(BTN_MODE_RGB))
	        	{
	        		UB_Led_On(LED_BO);
	        		Delay_ms(500);
	        		number_color++;
	        		if (number_color>=15)
	        		  {
	        			number_color=1;
	        		  };
	                settings.NumberColorF=number_color;
	                settings.F1=0;
	                settings.F2=0;
	                settings.F3=0;
	                FLASH_WriteSettings();

	        	}
	        	UB_Led_Off(LED_BO);

	        }

}
