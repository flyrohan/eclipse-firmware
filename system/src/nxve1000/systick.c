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

static struct __attribute__((__packed__)) _SYSTICK_ {
	uint64_t timestamp;
	uint32_t hz;
	uint32_t ratio;
	uint32_t lastdec;
} _systick = {
	.timestamp = 0,
};

void SysTick_Delay(int us)
{
	uint64_t start = SysTick_GetTick();

	while (1) {
		uint64_t curr = SysTick_GetTick();

		if (curr < start)
			continue;

		if ((curr - start) >= (uint64_t)us)
			break;
	};
}

uint64_t SysTick_GetTick(void)
{
	uint64_t time;
	uint32_t tick;

	tick = SysTick->VAL;
	__ISB();
	time = _systick.timestamp;

	return time + (uint64_t)(tick / _systick.ratio);
}

void SysTick_Handler(void)
{
	_systick.timestamp += _systick.hz;
}

void SysTick_Init(int hz)
{
	uint32_t ticks = SYSTEM_CLOCK / (uint32_t)hz;
	uint32_t ctrl;

	if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
	    return;

	_systick.ratio = SYSTEM_CLOCK / (uint32_t)(hz * hz);
	_systick.hz = (uint32_t)hz;
	ctrl = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
	SysTick->LOAD = (uint32_t)(ticks - 1UL);
	SysTick->VAL = 0UL;
	SysTick->CTRL = ctrl;
}

#endif
