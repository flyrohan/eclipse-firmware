#ifndef _TIMER_H_
#define _TIMER_H_

int  TIMER_Init(int ch, unsigned int infreq, unsigned int tfreq, int hz);
void TIMER_Start(int ch);
void TIMER_Stop(int ch);

uint64_t TIMER_GetTickUS(int ch);
void TIMER_Delay(int ch, int ms);

/* register to system time */
void TIMER_Register(int ch, unsigned int infreq, unsigned int tfreq, int hz);

#endif
