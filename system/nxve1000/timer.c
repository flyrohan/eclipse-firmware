#include <stdbool.h>
#include <stddef.h>
#include <cmsis_device.h>
#include <timer.h>
#include <systime.h>
#include <io.h>
#include <config.h>

#ifdef TIMER_ENABLED

#define TIMER_CHS				8
#define TIMER_CLOCK_HZ			1000000			/* 1Mhz */
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

static struct TIMER_t timer_t[TIMER_CHS];
static struct TIMER_t *systimer = NULL;

static uint64_t TIMER_GetTickUS(void)
{
	uint64_t time = systimer->timestamp;
	uint32_t lastdec = systimer->lastdec;
	uint32_t now = TIMER_MAX_COUNT - readl(&systimer->base->TCNTO);

	if (now >= lastdec)
		time += now - lastdec;
	else
		time += now + TIMER_MAX_COUNT - lastdec;

	systimer->lastdec = now;
	systimer->timestamp = time;

	return systimer->timestamp;
}

static void TIMER_Delay(int ms)
{
	uint64_t end = TIMER_GetTickUS() + (uint64_t)ms * 1000;

	while (TIMER_GetTickUS() < end) {
			;
	};
}

static SysTime_Op SysTick_Op __attribute__((unused)) = {
	.Delay = TIMER_Delay,
	.GetTickUS = TIMER_GetTickUS,
};

static void TIMER_Config(int ch, int mux, int scale, unsigned int count)
{
	struct TIMER_t *timer = &timer_t[ch];

	writel(_mask(timer->base->TCFG1, TCFG1_MUX_MASK) | (uint32_t)mux, &timer->base->TCFG1);
	writel(_mask(timer->base->TCFG0, TCFG0_PRESCALER_MASK) | (uint32_t)(scale - 1), &timer->base->TCFG0);
	writel(count, &timer->base->TCNTB);
	writel(count, &timer->base->TCMPB);
}

static void TIMER_Start(int ch, bool irqenb)
{
	struct TIMER_t *timer = &timer_t[ch];

	if (irqenb)
		writel(TINT_STATUS | TINT_ENABLE, &timer->base->TINT_CSTAT);

	writel((readl(&timer->base->TCON) | TCON_MANUALUPDATE), &timer->base->TCON);
	writel(TCON_AUTORELOAD | TCON_START, &timer->base->TCON);
}

static void TIMER_Stop(int ch)
{
	struct TIMER_t *timer = &timer_t[ch];

	writel(0x0, &timer->base->TINT_CSTAT);
	writel(_mask(timer->base->TCON, TCON_START), &timer->base->TCON);
}

int TIMER_Init(int ch, unsigned int clock, int hz __attribute__((unused)))
{
	struct TIMER_t *timer = &timer_t[ch];
	unsigned int count = TIMER_MAX_COUNT;
	bool irqenb = false;
	int scale = (int)clock / TIMER_CLOCK_HZ;
	
	if (ch > 7)
		return -1;

	timer->base = (void *)(TIMER_PHY_BASE + (TIMER_CH_OFFSET * ch));

	TIMER_Stop(ch);
	TIMER_Config(ch, TIMER_MUX_SEL, scale, count);
	TIMER_Start(ch, irqenb);

	return 0;
}

/* register to system time */
void TIMER_Register(int ch, unsigned int clock, int hz)
{
	TIMER_Init(ch, clock, hz);

	systimer = &timer_t[ch];

	SysTime_Register(&SysTick_Op);
}
#endif  /* TIMER_ENABLED */
