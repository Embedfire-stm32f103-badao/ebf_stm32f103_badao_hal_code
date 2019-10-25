#ifndef __INTERNAL_FLASH_H
#define	__INTERNAL_FLASH_H

#include "stm32f1xx.h"



//写入的起始地址与结束地址
#define WRITE_START_ADDR  ((uint32_t)0x08008000)
#define WRITE_END_ADDR    ((uint32_t)0x0800C000)



typedef enum 
{
	FAILED = 0, 
  PASSED = !FAILED
} TestStatus;



int InternalFlash_Test(void);



#endif /* __INTERNAL_FLASH_H */
