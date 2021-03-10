#ifndef _TIMER_H_
#define _TIMER_H_

void TIMER_Delay(int us);
uint64_t TIMER_GetTick(void);
int TIMER_Init(int ch, unsigned int clock);

#endif
