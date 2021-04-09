#ifndef _TIMER_H_
#define _TIMER_H_

int  TIMER_Init(int ch, unsigned int infreq, unsigned int tfreq, int hz);
/* register to system time */
void TIMER_Register(int ch, unsigned int infreq, unsigned int tfreq, int hz);

#endif
