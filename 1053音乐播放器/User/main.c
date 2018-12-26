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
#include "./sdio/bsp_sdio_sdcard.h"
#include "./sdio/sdio_test.h"
#include "./led/bsp_led.h"
#include "./FatFS/ff.h"
#include "./FatFS/ff_gen_drv.h"
#include "./FatFS/drivers/sd_diskio.h"
#include  "VS1053.h"
#include "string.h"
#include "flac.h"

static void printf_fatfs_error(FRESULT fresult);

uint8_t  buffer[BUFSIZE];

/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
char SDPath[4];                   /* SD卡逻辑设备路径 */
FATFS fs;													/* FatFs文件系统对象 */
FIL file;													/* 文件对象 */
FRESULT f_res;                    /* 文件操作结果 */
UINT bw;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{        
  /* 配置系统时钟为72 MHz */ 
  SystemClock_Config();
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();
  
	 /* 初始化LED灯 */
  LED_GPIO_Config();
	LED_BLUE;	
	
  /*初始化USART1*/
  DEBUG_USART_Config();
  
	/* 初始化调试串口，一般为串口1 */
	DEBUG_USART_Config();	
  printf("\r\r\n****** 这是一个SD卡 文件系统实验 ******\r\r\n");

  /* 注册一个FatFS设备：SD卡 */
  FATFS_LinkDriver(&SD_Driver, SDPath);

  //在SD卡挂载文件系统，文件系统挂载时会对SD卡初始化
  f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
  printf_fatfs_error(f_res);
  VS_Init(); 
  printf("vs1053:%4X\n",VS_Ram_Test());
  HAL_Delay(100);
  VS_Sine_Test();	
  VS_HD_Reset();
  VS_Soft_Reset();
	
	while(1)
	{
		// 测试歌曲放在外部FLASH W25Q64中
		vs1053_player_song("music/TestFile.mp3");
		printf("MusicPlay End\n");
	}	
}
//播放歌曲
void vs1053_player_song(uint8_t *filepath)
{
	uint16_t i=0;
	
	VS_Restart_Play();  					
	VS_Set_All();        							 
	VS_Reset_DecodeTime();
	
	if(strstr((const char*)filepath,".flac")||strstr((const char*)filepath,".FLAC"))
		VS_Load_Patch((uint16_t*)vs1053b_patch,VS1053B_PATCHLEN);
	
	f_res=f_open(&file,(const TCHAR*)filepath,FA_READ);

	if(f_res==0)
	{ 
		VS_SPI_SpeedHigh();				   
		while(1)
		{
			i=0;	
			f_res=f_read(&file,buffer,BUFSIZE,(UINT*)&bw);		
			do
			{  	
				if(VS_Send_MusicData(buffer+i)==0)
				{
					i+=32;
				}
			}while(i<bw);
			
			if(bw!=BUFSIZE||f_res!=0)
			{
				break;	  
			}
			LED2_TOGGLE;
		}
		 /* 注销一个FatFS设备：SD卡 */
  FATFS_UnLinkDriver(SDPath);
	}	  					     	  
}   

/**
  * @brief  打印输出信息
  * @param  无
  * @retval 无
  */
static void printf_fatfs_error(FRESULT fresult)
{
  switch(fresult)
  {
    case FR_OK:                   
      printf("》操作成功。\r\n");
    break;
    case FR_DISK_ERR:             
      printf("！！硬件输入输出驱动出错。\r\n");
    break;
    case FR_INT_ERR:              
      printf("！！断言错误。\r\n");
    break;
    case FR_NOT_READY:            
      printf("！！物理设备无法工作。\r\n");
    break;
    case FR_NO_FILE:              
      printf("！！无法找到文件。\r\n");
    break;
    case FR_NO_PATH:             
      printf("！！无法找到路径。\r\n");
    break;
    case FR_INVALID_NAME:         
      printf("！！无效的路径名。\r\n");
    break;
    case FR_DENIED:               
    case FR_EXIST:                
      printf("！！拒绝访问。\r\n");
    break;
    case FR_INVALID_OBJECT:       
      printf("！！无效的文件或路径。\r\n");
    break;
    case FR_WRITE_PROTECTED:      
      printf("！！逻辑设备写保护。\r\n");
    break;
    case FR_INVALID_DRIVE:        
      printf("！！无效的逻辑设备。\r\n");
    break;
    case FR_NOT_ENABLED:          
      printf("！！无效的工作区。\r\n");
    break;
    case FR_NO_FILESYSTEM:        
      printf("！！无效的文件系统。\r\n");
    break;
    case FR_MKFS_ABORTED:         
      printf("！！因函数参数问题导致f_mkfs函数操作失败。\r\n");
    break;
    case FR_TIMEOUT:              
      printf("！！操作超时。\r\n");
    break;
    case FR_LOCKED:               
      printf("！！文件被保护。\r\n");
    break;
    case FR_NOT_ENOUGH_CORE:      
      printf("！！长文件名支持获取堆空间失败。\r\n");
    break;
    case FR_TOO_MANY_OPEN_FILES:  
      printf("！！打开太多文件。\r\n");
    break;
    case FR_INVALID_PARAMETER:    
      printf("！！参数无效。\r\n");
    break;
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
  *            HSE PREDIV1                    = 1
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
