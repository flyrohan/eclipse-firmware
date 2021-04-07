#ifndef _TIMER_H_
#define _TIMER_H_

int  TIMER_Init(int ch, unsigned int clock, int hz);
/* register to system time */
void TIMER_Register(int ch, unsigned int clock, int hz);

#endif
