/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <cmsis_device.h>
#include <ExceptionHandlers.h>
#include <config.h>
#include <init.h>

#ifdef SYSTICK_ENABLED
#define SYSTICK_MAX_COUNT	((1 << 24) - 1)

static struct __attribute__((__packed__)) _SYSTICK_ {
	uint64_t timestamp;
	uint32_t lastdec;
	uint32_t ticksec;		/* tick_per_sec */
	uint32_t hz;
	uint32_t ratio;
} _systick = {
	.timestamp = 0,
};

static uint64_t SysTick_GetTick(void)
{
#if (SYSTICK_IRQ_MODE)
	uint64_t time;
	uint32_t tick;

	tick = SysTick->VAL;
	__ISB();
	time = _systick.timestamp;

	return time + (uint64_t)(tick / _systick.ratio);
#else
	uint64_t time = _systick.timestamp;
	uint32_t lastdec = _systick.lastdec;
	uint32_t now = SYSTICK_MAX_COUNT - SysTick->VAL;

	if (now >= lastdec)
		time += now - lastdec;
	else
		time += now + SYSTICK_MAX_COUNT - lastdec;

	_systick.lastdec = now;
	_systick.timestamp = time;

	return _systick.timestamp / _systick.ratio;
#endif
}

static void SysTick_Delay(int ms)
{
#if (SYSTICK_IRQ_MODE)
	uint64_t start = SysTick_GetTick();
	uint64_t wait = (uint64_t)ms * 1000;

	while (1) {
		uint64_t curr = SysTick_GetTick();

		if (curr < start)
			continue;

		if ((curr - start) >= (uint64_t)wait)
			break;
	};
#else
	uint64_t end = SysTick_GetTick() + (uint64_t)ms * 1000;

	while (SysTick_GetTick() < end) {
			;
	};
#endif
}

void SysTick_Handler(void)
{
	_systick.timestamp += _systick.hz;
}

int SysTick_Init(unsigned int clock, int hz)
{
	uint32_t ticksec = clock / (uint32_t)hz;

	if ((ticksec - 1UL) > SysTick_LOAD_RELOAD_Msk)
	    return -1;

	_systick.ticksec = ticksec;
	_systick.ratio = clock / (uint32_t)(hz * hz);
	_systick.hz = (uint32_t)hz;

	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);

	SysTick->VAL = 0UL;
#if SYSTICK_IRQ_MODE
	SysTick->LOAD = (uint32_t)(ticksec - 1UL);
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
#else
	SysTick->LOAD = (uint32_t)(SYSTICK_MAX_COUNT);
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
#endif

	return 0;
}

static SysTime_Op SysTick_Op = {
	.Delay = SysTick_Delay,
	.GetTick = SysTick_GetTick,
};

void SysTick_TimeInit(unsigned int clock, int hz)
{
	SysTick_Init(clock, hz);
	SysTime_Init(&SysTick_Op);
}
#endif
