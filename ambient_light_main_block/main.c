#include "uart.h"
#include "delay.h"
#include "nrf24.h"
#include "functions.h"
#include "stm32_ub_led.h"
#include "stm32_ub_button.h"
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


		uint32_t i,j,k;

		uint8_t  number_color = 1;

		uint16_t adc_value = 0;

        char buffer[30];

		// Buffer to store a payload of maximum width
		uint8_t nRF24_payload[32];

		// Pipe number
		nRF24_RXResult pipe;

		// Length of received payload
		uint8_t payload_length;



int main(void)
{

	        SetSysClockTo72();

	        UART_Init(115200);

			init_spi();

			Delay_Init();

			init_nrf24l01();

			UB_Led_Init();

			UB_Button_Init();

			init_pwm();

			init_adc();

			UB_Led_On(LED_DEBUG);

			set_color(Green, BRIGHTNESS_100);
			Delay_ms(500);


			UB_Led_On(LED_BO);

        	UB_Led_On(BUZZER);
        	Delay_ms(100);
        	UB_Led_Off(BUZZER);

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
	        	//UB_Led_Toggle(LED_DEBUG);
	        	//UB_Led_Toggle(BUZZER);

	        	adc_value = ADC_GetConversionValue(ADC1);


       	        //sprintf(buffer, "V=%d  B=%d\r\n", adc_value,set_brightness(adc_value));
       	        //UART_SendStr(buffer);

	        	//Delay_ms(50);


        		switch (number_color) {
        		case 1: set_color(Red,set_brightness(adc_value)); break;
        		case 2: set_color(Green,set_brightness(adc_value)); break;
        		case 3: set_color(Blue,set_brightness(adc_value)); break;
        		case 4: set_color(Yellow,set_brightness(adc_value)); break;
        		case 5: set_color(Orange,set_brightness(adc_value)); break;
        		case 6: set_color(Purple,set_brightness(adc_value)); break;
        		case 7: set_color(White,set_brightness(adc_value)); break;
        		case 8: set_color(Cyan,set_brightness(adc_value)); break;
        		}


	        	if (UB_Button_OnClick(BTN_MODE_RGB))
	        	{
	        		UB_Led_On(LED_BO);
	        		Delay_ms(100);
	        		number_color++;
	        		if (number_color==9) {number_color=1;};
	        	}
	        	UB_Led_Off(LED_BO);

	        }

}
