//--------------------------------------------------------------
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_button.h"



//--------------------------------------------------------------
// Внутренние функции
//--------------------------------------------------------------
#if BUTTON_USE_TIMER==1
  void P_Button_InitTIM(void);
  void P_Button_InitNVIC(void);
#endif




//--------------------------------------------------------------
// Определение всех кнопок
// Порядок как BUTTON_NAME_t
//
// Параметры: [GPIO_PuPd_NOPULL,GPIO_PuPd_UP,GPIO_PuPd_DOWN]
//--------------------------------------------------------------
BUTTON_t BUTTON[] = {
  // Name    ,PORT , PIN       , CLOCK              ,Opposition      , Status
  {BTN_MODE_RGB,GPIOB,GPIO_Pin_11 ,RCC_APB2Periph_GPIOB,GPIO_Mode_IPU, Bit_RESET},  // PA0 = Пользовательская кнопка на плате Discovery
};


//--------------------------------------------------------------
// Инициализация всех кнопок
//--------------------------------------------------------------
void UB_Button_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  BUTTON_NAME_t btn_name;
  
  for(btn_name=0;btn_name<BUTTON_ANZ;btn_name++) {
    // Включение тактирования
    RCC_APB2PeriphClockCmd(BUTTON[btn_name].BUTTON_CLK, ENABLE);
  
    // Конфигурация как цифровой вход
    GPIO_InitStructure.GPIO_Mode = BUTTON[btn_name].BUTTON_R;
    GPIO_InitStructure.GPIO_Pin = BUTTON[btn_name].BUTTON_PIN;
    GPIO_Init(BUTTON[btn_name].BUTTON_PORT, &GPIO_InitStructure);
  }

#if BUTTON_USE_TIMER==1
  // Инициализация Timer
  P_Button_InitTIM();
  // Инициализация NVIC
  P_Button_InitNVIC();
#endif
}


//--------------------------------------------------------------
// Чтение статуса кнопки (без фильтрации дребезга)
// Возвращаемое значение:
// - Когда кнопка отпущена = BTN_RELEASED
// - Когда кнопка нажата = BTN_PRESSED
//--------------------------------------------------------------
BUTTON_STATUS_t UB_Button_Read(BUTTON_NAME_t btn_name)
{
  uint8_t wert;

  wert=GPIO_ReadInputDataBit(BUTTON[btn_name].BUTTON_PORT, BUTTON[btn_name].BUTTON_PIN);
  if(wert==Bit_RESET) {
    return(BTN_RELEASED);
  }
  else {
    return(BTN_PRESSED);
  }
} 


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Кнопка нажата (защита от дребезга контактов)
// ret_wert = True, пока кнопка нажата
//--------------------------------------------------------------
bool UB_Button_OnPressed(BUTTON_NAME_t btn_name)
{
  uint8_t wert;

  wert=BUTTON[btn_name].BUTTON_AKT;

  if(wert==Bit_RESET) {
    return(false);
  }
  else {
    return(true);
  }
}
#endif


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Кнопка нажата (защита от дребезга контактов)
// ret_wert = True только один раз, в момент нажатия кнопки
//--------------------------------------------------------------
bool UB_Button_OnClick(BUTTON_NAME_t btn_name)
{
  uint8_t wert,old;
  static uint8_t old_wert[BUTTON_ANZ];

  wert=BUTTON[btn_name].BUTTON_AKT;
  old=old_wert[btn_name];
  old_wert[btn_name]=wert;

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


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Кнопка отпущена (защита от дребезга контактов)
// ret_wert = True только один раз, в момент отпускания кнопки
//--------------------------------------------------------------
bool UB_Button_OnRelease(BUTTON_NAME_t btn_name)
{
  uint8_t wert,old;
  static uint8_t old_wert[BUTTON_ANZ];

  wert=BUTTON[btn_name].BUTTON_AKT;
  old=old_wert[btn_name];
  old_wert[btn_name]=wert;

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


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Внутренняя функция
// Инициализация Timer
//--------------------------------------------------------------
void P_Button_InitTIM(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  // Включение тактирования
  RCC_APB1PeriphClockCmd(UB_BUTTON_TIM_CLK, ENABLE);

  // Инициализация Timer
  TIM_TimeBaseStructure.TIM_Period =  UB_BUTTON_TIM_PERIODE;
  TIM_TimeBaseStructure.TIM_Prescaler = UB_BUTTON_TIM_PRESCALE;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(UB_BUTTON_TIM, &TIM_TimeBaseStructure);

  // Запуск Timer
  TIM_ARRPreloadConfig(UB_BUTTON_TIM, ENABLE);
  TIM_Cmd(UB_BUTTON_TIM, ENABLE);
}
#endif


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Внутренняя функция
// Инициализация NVIC
//--------------------------------------------------------------
void P_Button_InitNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  //---------------------------------------------
  // Инициализация прерывания таймера
  //---------------------------------------------
  TIM_ITConfig(UB_BUTTON_TIM,TIM_IT_Update,ENABLE);

  // Конфигурирование NVIC
  NVIC_InitStructure.NVIC_IRQChannel = UB_BUTTON_TIM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
#endif


#if BUTTON_USE_TIMER==1
//--------------------------------------------------------------
// Прерывание от таймера
//--------------------------------------------------------------
void UB_BUTTON_TIM_ISR_HANDLER(void)
{
  BUTTON_NAME_t btn_name;
  uint8_t wert;

  // Есть только один источник прерывания
  TIM_ClearITPendingBit(UB_BUTTON_TIM, TIM_IT_Update);

  // Считывание всех кнопок
  for(btn_name=0;btn_name<BUTTON_ANZ;btn_name++) {
    wert=GPIO_ReadInputDataBit(BUTTON[btn_name].BUTTON_PORT, BUTTON[btn_name].BUTTON_PIN);
    BUTTON[btn_name].BUTTON_AKT=wert;
  }
}
#endif
