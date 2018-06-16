//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_led.h"


//--------------------------------------------------------------
// Определение всех светодиодов
// Объявление как LED_NAME_t
//
// Init : [LED_OFF,LED_ON]
//--------------------------------------------------------------
LED_t LED[] = {
  // Name    ,PORT , PIN       , CLOCK              , Init
  {LED_DEBUG ,GPIOC,GPIO_Pin_13,RCC_APB2Periph_GPIOC,LED_OFF},   // PD12=Зеленый светодиод на плате Discovery
  {LED_BO ,GPIOC,GPIO_Pin_14,RCC_APB2Periph_GPIOC,LED_OFF},   // PD12=Зеленый светодиод на плате Discovery
  {BUZZER ,GPIOB,GPIO_Pin_10,RCC_APB2Periph_GPIOB,LED_OFF},   // PD12=Зеленый светодиод на плате Discovery
};



//--------------------------------------------------------------
// Инициализация всех светодиодов
//--------------------------------------------------------------
void UB_Led_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  LED_NAME_t led_name;
  
  for(led_name=0;led_name<LED_ANZ;led_name++) {
    // Включение синхронизации
    RCC_APB2PeriphClockCmd(LED[led_name].LED_CLK, ENABLE);

    // Конфигурация порта как цифрового выхода
    GPIO_InitStructure.GPIO_Pin = LED[led_name].LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED[led_name].LED_PORT, &GPIO_InitStructure);

    // Установка значений по умолчанию
    if(LED[led_name].LED_INIT==LED_OFF) {
    	 UB_Led_Off(led_name);
    }
    else {
    	UB_Led_On(led_name);
    }
  }
}


//--------------------------------------------------------------
// Выключение светодиода
//--------------------------------------------------------------
void UB_Led_Off(LED_NAME_t led_name)
{
  //LED[led_name].LED_PORT->BSRR =1<<LED[led_name].LED_PIN;
  GPIO_ResetBits(LED[led_name].LED_PORT,LED[led_name].LED_PIN);

}

//--------------------------------------------------------------
// Включение светодиода
//--------------------------------------------------------------
void UB_Led_On(LED_NAME_t led_name)
{
	 //LED[led_name].LED_PORT->BRR = 1<<LED[led_name].LED_PIN;
	 GPIO_SetBits(LED[led_name].LED_PORT,LED[led_name].LED_PIN);
} 

//--------------------------------------------------------------
// Переключить светодиод
//--------------------------------------------------------------
void UB_Led_Toggle(LED_NAME_t led_name)
{
  LED[led_name].LED_PORT->ODR ^= LED[led_name].LED_PIN;
}

//--------------------------------------------------------------
// Инвертировать состояние светодиода
//--------------------------------------------------------------
void UB_Led_Switch(LED_NAME_t led_name, LED_STATUS_t wert)
{
  if(wert==LED_OFF) {
    UB_Led_Off(led_name);
  }
  else {
    UB_Led_On(led_name);
  }
}
