//--------------------------------------------------------------
// File     : stm32_ub_led.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F10_UB_LED_H
#define __STM32F10_UB_LED_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"




//--------------------------------------------------------------
// Р В Р Р‹Р В РЎвЂ”Р В РЎвЂ�Р РЋР С“Р В РЎвЂўР В РЎвЂќ Р В Р вЂ Р РЋР С“Р В Р’ВµР РЋРІР‚В¦ Р РЋР С“Р В Р вЂ Р В Р’ВµР РЋРІР‚С™Р В РЎвЂўР В РўвЂ�Р В РЎвЂ�Р В РЎвЂўР В РўвЂ�Р В РЎвЂўР В Р вЂ 
// Р В РЎСљР В РЎвЂўР В РЎпїЅР В Р’ВµР РЋР вЂљР В Р’В° Р В Р вЂ¦Р В Р’Вµ Р В РўвЂ�Р РЋРЎвЂњР В Р’В±Р В Р’В»Р В РЎвЂ�Р РЋР вЂљР РЋРЎвЂњР РЋР вЂ№Р РЋРІР‚С™Р РЋР С“Р РЋР РЏ, Р В Р вЂ¦Р В Р’В°Р РЋРІР‚РЋР В РЎвЂ�Р В Р вЂ¦Р В Р’В°Р РЋР вЂ№Р РЋРІР‚С™Р РЋР С“Р РЋР РЏ Р РЋР С“ 0
//--------------------------------------------------------------
typedef enum 
{
  LED_DEBUG = 0,  // LED4 Р В Р вЂ¦Р В Р’В° STM32F4-Discovery
  LED_BO = 1,  // LED4 Р Р…Р В° STM32F4-Discovery
  BUZZER = 2  // LED4 Р Р…Р В° STM32F4-Discovery
}LED_NAME_t;

#define  LED_ANZ  3 // Р В РЎв„ўР В РЎвЂўР В Р’В»Р В РЎвЂ�Р РЋРІР‚РЋР В Р’ВµР РЋР С“Р РЋРІР‚С™Р В Р вЂ Р В РЎвЂў LED_NAME_t


//--------------------------------------------------------------
// Р В РїС—Р…Р В Р вЂ¦Р В РўвЂ�Р В РЎвЂ�Р В РЎвЂќР В Р’В°Р РЋРІР‚С™Р В РЎвЂўР РЋР вЂљ Р РЋР С“Р В РЎвЂўР РЋР С“Р РЋРІР‚С™Р В РЎвЂўР РЋР РЏР В Р вЂ¦Р В РЎвЂ�Р РЋР РЏ Р РЋР С“Р В Р вЂ Р В Р’ВµР РЋРІР‚С™Р В РЎвЂўР В РўвЂ�Р В РЎвЂ�Р В РЎвЂўР В РўвЂ�Р В РЎвЂўР В Р вЂ 
//--------------------------------------------------------------
typedef enum {
  LED_OFF = 0,  // Р В Р Р‹Р В Р вЂ Р В Р’ВµР РЋРІР‚С™Р В РЎвЂўР В РўвЂ�Р В РЎвЂ�Р В РЎвЂўР В РўвЂ� Р В Р вЂ Р РЋРІР‚в„–Р В РЎвЂќР В Р’В»Р РЋР вЂ№Р РЋРІР‚РЋР В Р’ВµР В Р вЂ¦
  LED_ON         // Р В Р Р‹Р В Р вЂ Р В Р’ВµР РЋРІР‚С™Р В РЎвЂўР В РўвЂ�Р В РЎвЂ�Р В РЎвЂўР В РўвЂ� Р В Р вЂ Р В РЎвЂќР В Р’В»Р РЋР вЂ№Р РЋРІР‚РЋР В Р’ВµР В Р вЂ¦
}LED_STATUS_t;


//--------------------------------------------------------------
// Р В Р Р‹Р РЋРІР‚С™Р РЋР вЂљР РЋРЎвЂњР В РЎвЂќР РЋРІР‚С™Р РЋРЎвЂњР РЋР вЂљР В Р’В° Р В РЎвЂ�Р В Р вЂ¦Р В РЎвЂ�Р РЋРІР‚В Р В РЎвЂ�Р В Р’В°Р В Р’В»Р В РЎвЂ�Р В Р’В·Р В Р’В°Р РЋРІР‚В Р В РЎвЂ�Р В РЎвЂ� Р РЋР С“Р В Р вЂ Р В Р’ВµР РЋРІР‚С™Р В РЎвЂўР В РўвЂ�Р В РЎвЂ�Р В РЎвЂўР В РўвЂ�Р В Р’В°
//--------------------------------------------------------------
typedef struct {
  LED_NAME_t LED_NAME;    // Name
  GPIO_TypeDef* LED_PORT; // Port
  const uint16_t LED_PIN; // Pin
  const uint32_t LED_CLK; // Clock
  LED_STATUS_t LED_INIT;  // Init
}LED_t;


//--------------------------------------------------------------
// Р В РІР‚СљР В Р’В»Р В РЎвЂўР В Р’В±Р В Р’В°Р В Р’В»Р РЋР Р‰Р В Р вЂ¦Р РЋРІР‚в„–Р В Р’Вµ Р РЋРІР‚С›Р РЋРЎвЂњР В Р вЂ¦Р В РЎвЂќР РЋРІР‚В Р В РЎвЂ�Р В РЎвЂ�
//--------------------------------------------------------------
void UB_Led_Init(void);
void UB_Led_Off(LED_NAME_t led_name);
void UB_Led_On(LED_NAME_t led_name);
void UB_Led_Toggle(LED_NAME_t led_name);
void UB_Led_Switch(LED_NAME_t led_name, LED_STATUS_t wert);



//--------------------------------------------------------------
#endif // __STM32F4_UB_LED_H
