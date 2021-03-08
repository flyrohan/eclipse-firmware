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
static uint64_t systick_count = 0;
static uint32_t systick_inc_hz, systick_ratio_us;

void SysTick_Delay(int us)
{
	uint64_t s_us = SysTick_GetTick();

	while (1) {
		uint64_t c_us = SysTick_GetTick();

		if (c_us < s_us)
			continue;
		if ((c_us - s_us) >= (uint64_t)us)
			break;
	};
}

uint64_t SysTick_GetTick(void)
{
	return systick_count + (uint64_t)(SysTick->VAL / systick_ratio_us);
}

void SysTick_Handler(void)
{
	systick_count += systick_inc_hz;
}

void SysTick_Init(int hz)
{
	systick_inc_hz = (uint32_t)hz;
	systick_ratio_us = SYSTEM_CLOCK / (uint32_t)(hz * hz);

	SysTick_Config(SYSTEM_CLOCK / (uint32_t)hz);
}

#endif
