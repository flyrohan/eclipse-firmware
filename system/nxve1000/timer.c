#include <stdbool.h>
#include <stddef.h>
#include <cmsis_device.h>
#include <timer.h>
#include <systime.h>
#include <io.h>
#include <config.h>

#ifdef TIMER_ENABLED

#define TIMER_CHS				8
#ifndef TIMER_CLOCK_HZ
#define TIMER_CLOCK_HZ			1000000			/* 1Mhz */
#endif
#define	TIMER_MAX_COUNT			-(1UL)
#define TIMER_MUX_SEL			0 				/* bypass */

#define	TIMER_CH_OFFSET			(0x100)

#define TCFG0_PRESCALER_MASK	0xff
#define TCFG1_MUX_MASK          0x7
#define TCON_START				BIT(0)
#define TCON_MANUALUPDATE   	BIT(1)
#define TCON_INVERT         	BIT(2)
#define TCON_AUTORELOAD    		BIT(3)

#define TINT_STATUS    			BIT(5)
#define TINT_ENABLE    			BIT(0)

typedef struct {
  __IOM uint32_t TCFG0;			/* 0x00 */
  __IOM uint32_t TCFG1;			/* 0x04 */
  __IOM uint32_t TCON;			/* 0x08 */
  __IOM uint32_t TCNTB;			/* 0x0C */
  __IOM uint32_t TCMPB;			/* 0x10 */
  __IOM uint32_t TCNTO;			/* 0x14 */
  __IOM uint32_t TINT_CSTAT;	/* 0x18 */
} Timer_Reg;

/*
 * Timer HW
 */
struct TIMER_t {
	Timer_Reg *base;
	uint64_t timestamp;
	uint32_t lastdec;
};

static struct TIMER_t _timer[TIMER_CHS];

uint64_t TIMER_GetTickUS(int ch)
{
	uint64_t time = _timer[ch].timestamp;
	uint32_t lastdec = _timer[ch].lastdec;
	uint32_t now = TIMER_MAX_COUNT - readl(&_timer[ch].base->TCNTO);

	if (now >= lastdec)
		time += now - lastdec;
	else
		time += now + TIMER_MAX_COUNT - lastdec;

	_timer[ch].lastdec = now;
	_timer[ch].timestamp = time;

	return _timer[ch].timestamp;
}

void TIMER_Delay(int ch, int ms)
{
	uint64_t end = TIMER_GetTickUS(ch) + (uint64_t)ms * 1000;

	while (TIMER_GetTickUS(ch) < end) {
			;
	};
}

static void TIMER_Frequency(int ch, int mux, int scale, unsigned int count)
{
	struct TIMER_t *timer = &_timer[ch];

	writel(_mask(timer->base->TCFG1, TCFG1_MUX_MASK) |
			(uint32_t)mux, &timer->base->TCFG1);
	writel(_mask(timer->base->TCFG0, TCFG0_PRESCALER_MASK) |
			(uint32_t)(scale - 1), &timer->base->TCFG0);
	writel(count, &timer->base->TCNTB);
	writel(count, &timer->base->TCMPB);
}

void TIMER_Start(int ch)
{
	struct TIMER_t *timer = &_timer[ch];

	writel((readl(&timer->base->TCON) | TCON_MANUALUPDATE), &timer->base->TCON);
	writel(TCON_AUTORELOAD | TCON_START, &timer->base->TCON);
}

void TIMER_Stop(int ch)
{
	struct TIMER_t *timer = &_timer[ch];

	writel(0x0, &timer->base->TINT_CSTAT);
	writel(_mask(timer->base->TCON, TCON_START), &timer->base->TCON);
}

int TIMER_Init(int ch, unsigned int infreq, unsigned int tfreq, int hz)
{
	struct TIMER_t *timer = &_timer[ch];
	unsigned int count = TIMER_MAX_COUNT;
	int scale = (int)infreq / TIMER_CLOCK_HZ;
	
	if (ch > 7)
		return -1;

	if (hz)
		count = tfreq / (unsigned  int)hz;

	if (!tfreq)
		scale = (int)(infreq / tfreq);

	timer->base = (void *)(TIMER_PHY_BASE + (TIMER_CH_OFFSET * ch));

	TIMER_Stop(ch);
	TIMER_Frequency(ch, TIMER_MUX_SEL, scale, count);

	return 0;
}

static int systime_ch;
#define SysTime_Channel(_ch)		(systime_ch = _ch)
#define SysTime_GetChannel()		(systime_ch)

static uint64_t __SysTime_GetTickUS(void)
{
	return TIMER_GetTickUS(SysTime_GetChannel());
}

static void __SysTime_Delay(int ms)
{
	TIMER_Delay(SysTime_GetChannel(), ms);
}

static SysTime_Op Timer_Op __attribute__((unused)) = {
	.GetTickUS = __SysTime_GetTickUS,
	.Delay = __SysTime_Delay,
};

void TIMER_Register(int ch, unsigned int infreq, unsigned int tfreq, int hz)
{
	if (ch < 0 || ch > TIMER_CHS)
		return;

	SysTime_Channel(ch);
	SysTime_Register(&Timer_Op);

	TIMER_Init(ch, infreq, tfreq, hz);
	TIMER_Start(ch);
}
#endif  /* TIMER_ENABLED */
