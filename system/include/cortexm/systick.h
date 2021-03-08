#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "cmsis_device.h"

uint64_t SysTick_GetTick(void);
void SysTick_Delay (int ticks);
uint64_t SysTick_GetCounter(void);
void SysTick_Init(void);

void SysTick_ISRHandler(void);

#endif
