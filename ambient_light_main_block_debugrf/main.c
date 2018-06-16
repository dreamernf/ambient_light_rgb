#include "uart.h"
#include "delay.h"
#include "nrf24.h"
#include "functions.h"
#include "ws2812b.h"
#include "stm32_ub_led.h"
#include "stm32f10x_dma.h"
#include <stdio.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"



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

#define  count_pwm_steps    128

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


#define NUM_LEDS   1
#define  PERIOD   256


RGB_t leds[NUM_LEDS];


		uint8_t bright_rgb = 0;

		uint8_t  number_color = 1;

		uint16_t adc_value = 0;

		// Buffer to store a payload of maximum width
		uint8_t nRF24_payload[32];


		// Pipe number
		nRF24_RXResult pipe;

		// Length of received payload
		uint8_t payload_length = 5;


		void SetSysClockToHSE(void)
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
		        /* HCLK = SYSCLK */
		        RCC_HCLKConfig( RCC_SYSCLK_Div1);

		        /* PCLK2 = HCLK */
		        RCC_PCLK2Config( RCC_HCLK_Div1);

		        /* PCLK1 = HCLK */
		        RCC_PCLK1Config(RCC_HCLK_Div1);

		        /* Select HSE as system clock source */
		        RCC_SYSCLKConfig( RCC_SYSCLKSource_HSE);

		        /* Wait till PLL is used as system clock source */
		        while (RCC_GetSYSCLKSource() != 0x04)
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


void TIM4_IRQHandler(void)
		{
		        if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		        {
		            // ќб€зательно сбрасываем флаг
		            TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		            number_color++;
		            	        		if (number_color>=15)
		            	        		  {
		            	        			number_color=1;
		            	        		  };
		        }
		}


void set_color(struct RGB_COLOR_TYPE color, uint8_t brightness)
		{

			leds[0].r=(color.R*brightness)/(PERIOD-1);
			leds[0].g=(color.G*brightness)/(PERIOD-1);
			leds[0].b=(color.B*brightness)/(PERIOD-1);

			//leds[1].r=(color.R*brightness)/(PERIOD-1);
			//leds[1].g=(color.G*brightness)/(PERIOD-1);
			//leds[1].b=(color.B*brightness)/(PERIOD-1);

			//leds[2].r=(color.R*brightness)/(PERIOD-1);
			//leds[2].g=(color.G*brightness)/(PERIOD-1);
			//leds[2].b=(color.B*brightness)/(PERIOD-1);

		}


int main(void)
{

	        SetSysClockTo72();
	       // SetSysClockToHSE();

	        // TIMER4
	        TIM_TimeBaseInitTypeDef TIMER_InitStructure;
	        NVIC_InitTypeDef NVIC_InitStructure;

	            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	            TIM_TimeBaseStructInit(&TIMER_InitStructure);
	            TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	            TIMER_InitStructure.TIM_Prescaler = 64000;
	            TIMER_InitStructure.TIM_Period = 4000;
	            TIM_TimeBaseInit(TIM4, &TIMER_InitStructure);
	            TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	            TIM_Cmd(TIM4, ENABLE);

	            /* NVIC Configuration */
	               /* Enable the TIM4_IRQn Interrupt */
	               NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	               NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	               NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	               NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	               NVIC_Init(&NVIC_InitStructure);


	        Delay_Init();
	        UB_Led_Init();
   	        //FLASH_Init();
			ws2812b_Init();
			while (!ws2812b_IsReady()); // wait
			set_color(Green,RGB_PWM[127]);
			ws2812b_SendRGB(leds, NUM_LEDS);
			Delay_ms(1000);

			//UB_Button_Init();
			//init_adc();

			//UB_Led_On(LED_BO);
        	//UB_Led_On(BUZZER);
        	Delay_ms(500);
        	//UB_Led_Off(BUZZER);
        	//FLASH_ReadSettings();
        	number_color = 1;

			init_spi();

			init_nrf24l01();

			UB_Led_On(LED_DEBUG);

	        while (1) {

	        	bright_rgb = 120;

        		//номер цвета
	        	nRF24_payload[0] = 	number_color;

	        	//обща€ €ркость
	        	nRF24_payload[1] =  bright_rgb;

	        	//коэффициент €ркости передних фонариков
	        	nRF24_payload[2] = 	9;

	        	//коэффициент €ркости задних фонариков
	        	nRF24_payload[3] = 	9;

	        	//коэффициент €ркости подсветки ног
	        	nRF24_payload[4] = 	6;

	        	// Transmit a packet
	        	tx_res = nRF24_TransmitPacket(nRF24_payload, payload_length);

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

        		ws2812b_SendRGB(leds, NUM_LEDS);

	        	Delay_ms(50);




	        }

}
