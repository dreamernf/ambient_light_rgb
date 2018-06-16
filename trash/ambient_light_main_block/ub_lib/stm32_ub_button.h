//--------------------------------------------------------------
// File     : stm32_ub_button.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F10_UB_BUTTON_H
#define __STM32F10_UB_BUTTON_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"



//--------------------------------------------------------------
// Включение и отключение функции фильтрации дребезга контактов
// 1 = активна, 0 = неактивна
//--------------------------------------------------------------
#define  BUTTON_USE_TIMER      1  // Функция по таймеру


#if BUTTON_USE_TIMER==1
  #include "stm32f10x_tim.h"
  #include "misc.h"
  #include <stdbool.h>
#endif


//--------------------------------------------------------------
// Список всех кнопок
// Не дублируются, нумерация начинается с 0
//--------------------------------------------------------------
typedef enum 
{
  BTN_MODE_RGB = 0,    // BTN1 auf dem STM32F4-Discovery
}BUTTON_NAME_t;

#define  BUTTON_ANZ   1 // Anzahl von Button_NAME_t


//--------------------------------------------------------------
// Статус кнопки
//--------------------------------------------------------------
typedef enum {
  BTN_RELEASED = 0,  // Кнопка не нажата
  BTN_PRESSED        // Кнопка нажата
}BUTTON_STATUS_t;



//--------------------------------------------------------------
// Таймер обработки кнопок => TIM7
// Частота тактирования = 2*APB1 (APB1=42MHz) => TIM_CLK=84MHz
// TIM_Frq = TIM_CLK/период+1)/(делитель+1)
// TIM_Frq = 20Hz => 50ms (устанавливается не менее 1мс)
//--------------------------------------------------------------
#if BUTTON_USE_TIMER==1
  #define   UB_BUTTON_TIM              TIM4
  #define   UB_BUTTON_TIM_CLK          RCC_APB1Periph_TIM4
  #define   UB_BUTTON_TIM_PERIODE      4999
  #define   UB_BUTTON_TIM_PRESCALE     839
  #define   UB_BUTTON_TIM_IRQ          TIM4_IRQn
  #define   UB_BUTTON_TIM_ISR_HANDLER  TIM4_IRQHandler
#endif



//--------------------------------------------------------------
// Структура конфигурирования кнопки
//--------------------------------------------------------------
typedef struct {
  BUTTON_NAME_t BUTTON_NAME;  // Name
  GPIO_TypeDef* BUTTON_PORT;  // Port
  const uint16_t BUTTON_PIN;  // Pin
  const uint32_t BUTTON_CLK;  // Clock
  GPIOMode_TypeDef BUTTON_R;  // Widerstand
  uint8_t BUTTON_AKT;         // Istwert
}BUTTON_t;


//--------------------------------------------------------------
// Глобальная функция
//--------------------------------------------------------------
void UB_Button_Init(void);
BUTTON_STATUS_t UB_Button_Read(BUTTON_NAME_t btn_name);
#if BUTTON_USE_TIMER==1
  bool UB_Button_OnPressed(BUTTON_NAME_t btn_name);
  bool UB_Button_OnClick(BUTTON_NAME_t btn_name);
  bool UB_Button_OnRelease(BUTTON_NAME_t btn_name);
#endif

//--------------------------------------------------------------
#endif // __STM32F10_UB_BUTTON_H
