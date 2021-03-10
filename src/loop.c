#include <stdlib.h>
#include <config.h>

#ifdef LOOP_ENABLED

#ifdef SYSTICK_ENABLED
#define Delay(_d)	SysTick_Delay(_d * 1000)
#define GetTick()	(SysTick_GetTick() / 1000)
#else
#define Delay(_d)	TIMER_Delay(_d * 1000)
#define GetTick()	(TIMER_GetTick() / 1000)
#endif

#define CRTLC_EXIT() do { \
	int c = Getc();	\
	if (isCtrlc(c))	\
		return 0;	\
	} while (0)

/*
 * $> loop <sleep:ms> <count>
 */
static int do_loop(int argc, char * const argv[])
{
    int count = 10;
	int ms = 0;

	if (argc < 2)
		return -1;

	ms = (int)strtoul(argv[1], NULL, 10);

	if (argc > 2 && argv[2])
		count = (int)strtoul(argv[2], NULL, 10);

	for (int i = 0; i < count; i++)	{
		uint32_t tick = (uint32_t)GetTick();

		if (Tstc())
			CRTLC_EXIT();	
		Printf("[%03d] %d : %d\r\n", i, tick, ms);
		if (ms)
			Delay(ms);
	};

	return 0;
}

CMD_DEFINE(loop, do_loop);
#endif
