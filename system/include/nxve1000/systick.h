#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "cmsis_device.h"

void SysTick_Delay(int us);
uint64_t SysTick_GetTick(void);
void SysTick_Init(int hz);

#endif
