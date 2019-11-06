/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./ap3216c/ap3216c.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  /**/
  float ALS;
  uint16_t PS;
  uint16_t IR;
  uint8_t IntStatus;
  
  HAL_Init();        
  /* 配置系统时钟为180 MHz */ 
  SystemClock_Config();
  /* 初始化内核延时 */
  HAL_InitTick(5);
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();
  
  printf("\r\n 欢迎使用野火 STM32 F429 开发板。\r\n");	
  printf("\r\n 这是一个三合一光照传感器测试例程 \r\n");
  
  printf(" 芯片初始化中.....\n");
  /* 初始化 光照传感器 */
  ap3216c_init();

  while(1)    
  {
    IntStatus = ap3216c_get_IntStatus();    // 先读状态位，读ADC数据位会清除状态位（默认设置）
    ALS = ap3216c_read_ambient_light();
    PS = ap3216c_read_ps_data();
    IR = ap3216c_read_ir_data();

    printf("\n光照强度是：%.2fLux\n红外强度是：%d\n", ALS, IR);

    if (PS == 55555)    // IR 太强 PS 数据无效
      printf("IR 太强 PS 数据无效\n");
    else
    {
      printf("接近距离是：%d\n", PS & 0x3FF);
    }
    
    if (AP_INT_Read() == 0)
      printf("有中断产生\n");
    
    if ((PS >> 15) & 1)
      printf("物体接近\n");
    else
      printf("物体远离\n");
    
    if (IntStatus & 0x1)
      printf("ALS 产生中断\n");
    
    if (IntStatus >> 1 & 0x1)
      printf("PS 产生中断\n");
    
    HAL_Delay(400);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
