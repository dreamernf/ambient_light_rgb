#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static void prvSetupHardware( void );
void vTaskLED1(void *pvParameters);



void prvSetupHardware()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
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

void vTaskLED1(void *pvParameters) {

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

	xTaskCreate(vTaskLED1,"LED1",configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY + 1, NULL);

/* Start the scheduler. */
	vTaskStartScheduler();
    while(1);
}
