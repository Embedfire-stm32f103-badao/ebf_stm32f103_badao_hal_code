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
  * 实验平台:野火  STM32 霸道 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./internal_flash/bsp_internal_flash.h"   
#include "./usart/bsp_debug_usart.h"





/**
  * @brief  InternalFlash_Test,对内部FLASH进行读写测试
  * @param  None
  * @retval None
  */
int InternalFlash_Test(void)
{

	uint32_t Address = 0x00;				//记录写入的地址
	uint32_t DATA_32 = 0x3210ABCD;			//记录写入的数据
	uint32_t NbrOfPage = 0x00;			//记录写入多少页
	 __IO uint32_t Data32 = 0;
	
	
	uint32_t SECTORError = 0;
	TestStatus MemoryProgramStatus = PASSED;//记录整个测试结果
	

	static FLASH_EraseInitTypeDef EraseInitStruct;
  /* 解锁 */
	HAL_FLASH_Unlock();

  /* 计算要擦除多少页 */
  NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.NbPages       = NbrOfPage;
	EraseInitStruct.PageAddress   = WRITE_START_ADDR;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	{
		/*擦除出错，返回，实际应用中可加入处理 */
		return -1;
	}
  /* 向内部FLASH写入数据 */
	Address = WRITE_START_ADDR;
	while (Address < WRITE_END_ADDR)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
		{
		  Address = Address + 4;
		}
		else
		{ 
		  /*写入出错，返回，实际应用中可加入处理 */
				return -1;
		}
	}
	
  HAL_FLASH_Lock();

  /* 检查写入的数据是否正确 */
  Address = WRITE_START_ADDR;

  while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
  {
    if((*(__IO uint32_t*) Address) != DATA_32)
    {
      MemoryProgramStatus = FAILED;
    }
    Address += 4;
  }
	return MemoryProgramStatus;
}




