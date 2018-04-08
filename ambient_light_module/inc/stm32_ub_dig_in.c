//--------------------------------------------------------------
// File     : stm32_ub_dig_in.c
// Datum    : 07.10.2013
// Version  : 1.2
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.4
// GCC      : 4.7 2012q4
// Module   : GPIO, (TIM, MISC)
// Funktion : Digital_In Funktionen
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_dig_in.h"


//--------------------------------------------------------------
// внутренние функции
//--------------------------------------------------------------
#if DIN_USE_TIMER==1
  void P_DIN_InitTIM(void);
  void P_DIN_InitNVIC(void);
#endif




//--------------------------------------------------------------
// Определение всех пинов цифровых входов
// Определение как DIN_NAME_t
//
// Инициализация: [GPIO_PuPd_NOPULL,GPIO_PuPd_UP,GPIO_PuPd_DOWN]
//--------------------------------------------------------------
DIN_PIN_t DIN_PIN[] = {
  // Name    ,PORT , PIN       , CLOCK              ,Widerstand , Status
  {DIN_8  ,GPIOA,GPIO_Pin_11 ,RCC_APB2Periph_GPIOA,GPIO_Mode_IPD,Bit_RESET},
  {DIN_4  ,GPIOA,GPIO_Pin_12 ,RCC_APB2Periph_GPIOA,GPIO_Mode_IPD,Bit_RESET},
  {DIN_1  ,GPIOA,GPIO_Pin_10 ,RCC_APB2Periph_GPIOA,GPIO_Mode_IPD,Bit_RESET},
  {DIN_2  ,GPIOA,GPIO_Pin_15 ,RCC_APB2Periph_GPIOA,GPIO_Mode_IPD,Bit_RESET},
};

//--------------------------------------------------------------
// Инициализация всех пинов цифровых входов
//--------------------------------------------------------------
void UB_DigIn_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  DIN_NAME_t dig_pin;
  
  for(dig_pin=0;dig_pin<DIN_ANZ;dig_pin++) {
    // Включение тактирования
	RCC_APB2PeriphClockCmd(DIN_PIN[dig_pin].GPIO_CLK, ENABLE);
  
    // Конфигурирование как цифровые входы
    GPIO_InitStructure.GPIO_Mode = DIN_PIN[dig_pin].GPIO_R;
    GPIO_InitStructure.GPIO_Pin = DIN_PIN[dig_pin].GPIO_PIN;
    GPIO_Init(DIN_PIN[dig_pin].GPIO_PORT, &GPIO_InitStructure);   
  }

  #if DIN_USE_TIMER==1
    // Инициализация Timer
    P_DIN_InitTIM();
    // Инициализация NVIC
    P_DIN_InitNVIC();
  #endif
}


//--------------------------------------------------------------
// Чтение уровня на входе (без фильтрации дребезга)
// Возвращаемое значение:
// - Когда уровень низкий = Bit_RESET
// - Когда уровень высокий = Bit_SET
//--------------------------------------------------------------
BitAction UB_DigIn_Read(DIN_NAME_t dig_pin)
{
  BitAction wert;

  wert=GPIO_ReadInputDataBit(DIN_PIN[dig_pin].GPIO_PORT, DIN_PIN[dig_pin].GPIO_PIN);
  return(wert);
} 


#if DIN_USE_TIMER==1
//--------------------------------------------------------------
// Высокий уровень на входе (защита от дребезга контактов)
// ret_wert = True, пока на входе высокий уровень
//--------------------------------------------------------------
bool UB_DigIn_OnHiLevel(DIN_NAME_t dig_pin)
{
  uint8_t wert;

  wert=DIN_PIN[dig_pin].GPIO_AKT;

  if(wert==Bit_RESET) {
    return(false);
  }
  else {
    return(true);
  }
}
#endif


#if DIN_USE_TIMER==1
//--------------------------------------------------------------
// Фронт высокого уровня на входе (защита от дребезга контактов)
// ret_wert = True, однократно при фиксировании высокого уровня по фронту сигнала
//--------------------------------------------------------------
bool UB_DigIn_OnHiEdge(DIN_NAME_t dig_pin)
{
  uint8_t wert,old;
  static uint8_t old_wert[DIN_ANZ];

  wert=DIN_PIN[dig_pin].GPIO_AKT;
  old=old_wert[dig_pin];
  old_wert[dig_pin]=wert;

  if(wert==Bit_RESET) {
    return(false);
  }
  else if(old!=Bit_RESET) {
    return(false);
  }
  else {
    return(true);
  }

}
#endif


#if DIN_USE_TIMER==1
//--------------------------------------------------------------
// Фронт низкого уровня на входе (защита от дребезга контактов)
// ret_wert = True, однократно при фиксировании низкого уровня по фронту сигнала
//--------------------------------------------------------------
bool UB_DigIn_OnLoEdge(DIN_NAME_t dig_pin)
{
  uint8_t wert,old;
  static uint8_t old_wert[DIN_ANZ];

  wert=DIN_PIN[dig_pin].GPIO_AKT;
  old=old_wert[dig_pin];
  old_wert[dig_pin]=wert;

  if(wert!=Bit_RESET) {
    return(false);
  }
  else if(old==Bit_RESET) {
    return(false);
  }
  else {
    return(true);
  }

}
#endif


#if DIN_USE_TIMER==1
//--------------------------------------------------------------
// Внутренняя функция
// Инициализация Timer
//--------------------------------------------------------------
void P_DIN_InitTIM(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  // Включение тактирования
  RCC_APB1PeriphClockCmd(UB_DIN_TIM_CLK, ENABLE);

  // Инициализация Timer
  TIM_TimeBaseStructure.TIM_Period =  UB_DIN_TIM_PERIODE;
  TIM_TimeBaseStructure.TIM_Prescaler = UB_DIN_TIM_PRESCALE;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(UB_DIN_TIM, &TIM_TimeBaseStructure);

  // Запуск Timer
  TIM_ARRPreloadConfig(UB_DIN_TIM, ENABLE);
  TIM_Cmd(UB_DIN_TIM, ENABLE);
}
#endif


#if DIN_USE_TIMER==1
//--------------------------------------------------------------
// Внутренняя функция
// Инициализация NVIC
//--------------------------------------------------------------
void P_DIN_InitNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  //---------------------------------------------
  // Инициализация прерывания таймера
  //---------------------------------------------
  TIM_ITConfig(UB_DIN_TIM,TIM_IT_Update,ENABLE);

  // Конфигурирование NVIC
  NVIC_InitStructure.NVIC_IRQChannel = UB_DIN_TIM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
#endif


#if DIN_USE_TIMER==1
//--------------------------------------------------------------
// Прерывание от таймера
//--------------------------------------------------------------
void UB_DIN_TIM_ISR_HANDLER(void)
{
  DIN_NAME_t dig_pin;
  uint8_t wert;

  // Есть только один источник прерывания
  TIM_ClearITPendingBit(UB_DIN_TIM, TIM_IT_Update);

  // Читать все входы
  for(dig_pin=0;dig_pin<DIN_ANZ;dig_pin++) {
    wert=GPIO_ReadInputDataBit(DIN_PIN[dig_pin].GPIO_PORT, DIN_PIN[dig_pin].GPIO_PIN);
    DIN_PIN[dig_pin].GPIO_AKT=wert;
  }
}
#endif

