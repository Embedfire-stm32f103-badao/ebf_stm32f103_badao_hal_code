#ifndef __CLKCONFIG_H
#define	__CLKCONFIG_H


#include "stm32f1xx.h"


void HSE_SetSysClock(uint32_t pllmul);
void HSI_SetSysClock(uint32_t pllmul);

#endif /* __CLKCONFIG_H */
