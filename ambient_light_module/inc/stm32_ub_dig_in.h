//--------------------------------------------------------------
// File     : stm32_ub_dig_in.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_DIG_IN_H
#define __STM32F4_UB_DIG_IN_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


//--------------------------------------------------------------
// Включение и отключение функции противодребезговой защиты с таймером
// 1 = включена, 0 = не включена
//--------------------------------------------------------------
#define  DIN_USE_TIMER     0  // Функция по таймеру


#if DIN_USE_TIMER==1
  #include "stm32f10x_tim.h"
  #include "misc.h"
  #include <stdbool.h>
#endif



//--------------------------------------------------------------
// Список всех цифровых входов
// (Нумерация не дублируется и начинается с 0)
//--------------------------------------------------------------
typedef enum 
{
  DIN_1 = 0,
  DIN_2 = 1,
  DIN_4 = 2,
  DIN_8 = 3,
}DIN_NAME_t;

#define  DIN_ANZ   4 // Количество портов в DIN_NAME_t



//--------------------------------------------------------------
// Таймер обработки кнопок => TIM7
// Частота тактирования = 2*APB1 (APB1=42MHz) => TIM_CLK=84MHz
// TIM_Frq = TIM_CLK/период+1)/(делитель+1)
// TIM_Frq = 20Hz => 50ms (устанавливается не менее 1мс)
//--------------------------------------------------------------
#if DIN_USE_TIMER==1
  #define   UB_DIN_TIM              TIM4
  #define   UB_DIN_TIM_CLK          RCC_APB1Periph_TIM4
  #define   UB_DIN_TIM_PERIODE      4999
  #define   UB_DIN_TIM_PRESCALE     839
  #define   UB_DIN_TIM_IRQ          TIM4_IRQn
  #define   UB_DIN_TIM_ISR_HANDLER  TIM4_IRQHandler
#endif



//--------------------------------------------------------------
// Структура конфигурирования цифровых входов
//--------------------------------------------------------------
typedef struct {
  DIN_NAME_t GPIO_NAME;    // Name
  GPIO_TypeDef* GPIO_PORT; // Port
  const uint16_t GPIO_PIN; // Pin
  const uint32_t GPIO_CLK; // Clock
  GPIOMode_TypeDef GPIO_R; // Widerstand
  uint8_t GPIO_AKT;        // Istwert
}DIN_PIN_t;


//--------------------------------------------------------------
// Глобальная функция
//--------------------------------------------------------------
void UB_DigIn_Init(void);
BitAction UB_DigIn_Read(DIN_NAME_t dig_pin);
#if DIN_USE_TIMER==1
  bool UB_DigIn_OnHiLevel(DIN_NAME_t dig_pin);
  bool UB_DigIn_OnHiEdge(DIN_NAME_t dig_pin);
  bool UB_DigIn_OnLoEdge(DIN_NAME_t dig_pin);
#endif


//--------------------------------------------------------------
#endif // __STM32F4_UB_DIG_IN_H
