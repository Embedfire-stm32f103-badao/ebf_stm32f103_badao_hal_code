/**
  ******************************************************************************
  * @file    bsp_internalFlash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   内部FLASH读写测试范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 霸道 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./internal_flash/bsp_internal_flash.h"   
#include "./led/bsp_led.h"   


#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_32   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_48    /* End @ of user Flash area */

#define DATA_32                 ((uint32_t)0x12345678)

uint32_t Address = 0, PAGEError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;

/*V用于擦除过程的变量*/
 FLASH_EraseInitTypeDef EraseInitStruct;


/**
  * @brief  InternalFlash_Test,对内部FLASH进行读写测试
  * @param  None
  * @retval None
  */
void InternalFlash_Test(void)
{
	/* 解锁Flash以启用闪存控制寄存器访问*/
  HAL_FLASH_Unlock();

  /* 删除用户Flash区域*/
  /* 填充EraseInit结构*/
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
  EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  {
    
    while (1)
    {
      LED1_ON;
      printf("擦除失败\r\n");
    }
  }

  /* 逐字编程用户Flash区域*/

  Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
    {
      Address = Address + 4;
    }
    else
    {
      while (1)
      {
         LED1_ON;
         printf("写入失败\r\n");
      }
    }
  }

  /*锁定闪存以禁用闪存控制寄存器访问（推荐保护FLASH存储器免受可能的意外操作）*/
  HAL_FLASH_Lock();

  /* 检查编程数据是否正常
        MemoryProgramStatus = 0：数据编程正确
        MemoryProgramStatus！= 0：未正确编程的字数*/
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = 0x0;

  while (Address < FLASH_USER_END_ADDR)
  {
    data32 = *(__IO uint32_t *)Address;

    if (data32 != DATA_32)
    {
      MemoryProgramStatus++;
    }
    Address = Address + 4;
  }

  /*检查程序数据是否存在问题*/
  if (MemoryProgramStatus == 0)
  {
    printf("写入成功\r\n");
    LED2_ON;
  }
  else
  {
    while (1)
    {
      LED1_ON;
      printf("失败\r\n");
    }
  }
  while (1)
  {
  }
}
