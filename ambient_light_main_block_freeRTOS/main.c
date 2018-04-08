#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//#include "uart.h"
#include "delay.h"
//#include "nrf24.h"
#include "functions.h"
#include "ws2812b.h"
#include "stm32_ub_led.h"
#include "stm32_ub_button.h"
//#include "stm32f10x_flash.h"
#include "stm32f10x_dma.h"
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


void set_color(struct RGB_COLOR_TYPE color, uint8_t brightness)
		{

			leds[0].r=(color.R*brightness)/(PERIOD-1);
			leds[0].g=(color.G*brightness)/(PERIOD-1);
			leds[0].b=(color.B*brightness)/(PERIOD-1);

		}

static void prvSetupHardware( void );
void vTaskTest(void *pvParameters);



void prvSetupHardware()
{
	Delay_Init();


	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    //SetSysClockTo72();

	ws2812b_Init();

	while (!ws2812b_IsReady()); // wait

	set_color(Red,RGB_PWM[127]);
	ws2812b_SendRGB(leds, NUM_LEDS);
	Delay_ms(100);
}

/*******************************************************************/
void vApplicationIdleHook( void )
{
}

/*******************************************************************/
void vApplicationMallocFailedHook( void )
{
    for( ;; );
}

/*******************************************************************/
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    for( ;; );
}

/*******************************************************************/
void vApplicationTickHook( void )
{
}

/*******************************************************************/

void vTaskTest(void *pvParameters) {

        for (;;) {
                GPIO_SetBits(GPIOC, GPIO_Pin_13);
                vTaskDelay(500);
                GPIO_ResetBits(GPIOC, GPIO_Pin_13);
                vTaskDelay(500);
        }

}


int main(void)
{
	prvSetupHardware();

	xTaskCreate(vTaskTest,"TEST_BLINK",configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY + 1, NULL);

/* Start the scheduler. */
	vTaskStartScheduler();
    while(1);
}
