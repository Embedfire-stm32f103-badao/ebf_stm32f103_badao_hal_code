
#include "bsp_advance_tim.h" 

TIM_HandleTypeDef TIM_TimeBaseStructure;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 定时器通道功能引脚端口时钟使能 */
  ADVANCED_TIM_GPIO_RCC_CLK_ENABLE();
  
  /* 定时器通道1功能引脚IO初始化 */
  GPIO_InitStruct.Pin =  ADVANCE_OCPWM_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ADVANCE_OCPWM_GPIO_PORT, &GPIO_InitStruct);
  /* 定时器通道1互补通道功能引脚IO初始化 */
  GPIO_InitStruct.Pin = ADVANCE_OCNPWM_PIN;
  HAL_GPIO_Init(ADVANCE_OCNPWM_GPIO_PORT, &GPIO_InitStruct);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  /* 基本定时器外设时钟使能 */
  ADVANCED_TIM_RCC_CLK_ENABLE(); 
}
///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM信号 周期和占空比的计算--------------- */
// ARR ：自动重装载寄存器的值
// CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
// PWM 信号的周期 T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// 占空比P=CCR/(ARR+1)

void ADVANCE_TIM_Mode_Config(void)
{
  TIM_ClockConfigTypeDef Clock_Config;
  TIM_MasterConfigTypeDef Master_Config;
  TIM_BreakDeadTimeConfigTypeDef BreakDeadTime_Config;
  TIM_OC_InitTypeDef OC_Config;
  
  TIM_TimeBaseStructure.Instance = ADVANCED_TIMx;
  TIM_TimeBaseStructure.Init.Prescaler = ADVANCED_TIM_PRESCALER;
  TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM_TimeBaseStructure.Init.Period = ADVANCED_TIM_PERIOD;
  TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  TIM_TimeBaseStructure.Init.RepetitionCounter = ADVANCED_TIM_REPETITIONCOUNTER;
  HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

  Clock_Config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure, &Clock_Config);

  HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);
  
  Master_Config.MasterOutputTrigger = TIM_TRGO_RESET;
  Master_Config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure, &Master_Config);
  
  BreakDeadTime_Config.OffStateRunMode = TIM_OSSR_DISABLE;
  BreakDeadTime_Config.OffStateIDLEMode = TIM_OSSI_DISABLE;
  BreakDeadTime_Config.LockLevel = TIM_LOCKLEVEL_OFF;
  BreakDeadTime_Config.DeadTime = 0;
  BreakDeadTime_Config.BreakState = TIM_BREAK_DISABLE;
  BreakDeadTime_Config.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  BreakDeadTime_Config.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  HAL_TIMEx_ConfigBreakDeadTime(&TIM_TimeBaseStructure, &BreakDeadTime_Config);

  OC_Config.OCMode = TIM_OCMODE_PWM1;
  OC_Config.Pulse = 900;
  OC_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
  OC_Config.OCNPolarity = TIM_OCNPOLARITY_LOW;
  OC_Config.OCFastMode = TIM_OCFAST_DISABLE;
  OC_Config.OCIdleState = TIM_OCIDLESTATE_RESET;
  OC_Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  
  HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure, &OC_Config, TIM_CHANNEL_1);
}


/*********************************************END OF FILE**********************/
