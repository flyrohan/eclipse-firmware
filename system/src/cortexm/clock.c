/* Copyright (C) 2011 ARM Ltd. All rights reserved. */
/* A small implementation of the clock function using the performance counters
* available on the Cortex-M3 processor. */
/* The cpu cycle counter (SysTick) in Cortex-M3 counts down from 0xFFFFFF and an
* overflow interrupt is generated when the value reaches 0. The clock function
* relies on the startup code to install an interrupt handler to catch the
* interrupt and update the overflow counter. */
/* Set the clock frequency appropriately for your board. */

#include <clock.h>

/* SysTick registers */
/* SysTick control & status */
#define INITCPU_SYST_CSR ((volatile unsigned int *)0xE000E010)
/* SysTick Reload value */
#define INITCPU_SYST_RVR ((volatile unsigned int *)0xE000E014)
/* SysTick Current value */
#define INITCPU_SYST_CVR ((volatile unsigned int *)0xE000E018)
/* SysTick CSR register bits */
#define INITCPU_SYST_CSR_COUNTFLAG (1 << 16)
#define INITCPU_SYST_CSR_CLKSOURCE (1 << 2)
#define INITCPU_SYST_CSR_TICKINT (1 << 1)
#define INITCPU_SYST_CSR_ENABLE (1 << 0)
static volatile unsigned int systick_overflows = 0;

static void reset_cycle_counter(void)
{
	/* Set the reload value and clear the current value. */
	*INITCPU_SYST_RVR = 0x00ffffff;
	*INITCPU_SYST_CVR = 0;
	/* Reset the overflow counter */
	systick_overflows = 0;
}
static void start_cycle_counter(void)
{
	/* Enable the SysTick timer and enable the SysTick overflow interrupt */
	*INITCPU_SYST_CSR |=
	(INITCPU_SYST_CSR_CLKSOURCE |
	INITCPU_SYST_CSR_ENABLE |
	INITCPU_SYST_CSR_TICKINT);
}

static uint64_t get_cycle_counter(void)
{
	unsigned int overflows = systick_overflows;
	/* A systick overflow might happen here */
	unsigned int systick_count = *INITCPU_SYST_CVR;
	/* check if it did and reload the low bit if it did */
	unsigned int new_overflows = systick_overflows;
	if (overflows != new_overflows)
	{
		/* This suffices as long as there is no chance that a second
		overflow can happen since new_overflows was read */
		systick_count = *INITCPU_SYST_CVR;
		overflows = new_overflows;
	}
	/* Recall that the SysTick counter counts down. */
	return (((uint64_t)overflows << 18) + (0x00FFFFFF - systick_count));
}

static uint64_t cycle_counter_init_value;
clock_t clock(void)
{
	return (clock_t) (((get_cycle_counter() -
			cycle_counter_init_value) *
			CLOCKS_PER_SEC) / CPU_MHZ);
}

/* Microlib calls the _clock_init() function during library initialization to
* set up the cycle_counter_init_value variable. The value of this variable is
* then used as the baseline for subsequent calls to the clock() function. */
void clock_init(void)
{
	reset_cycle_counter();
	start_cycle_counter();
	cycle_counter_init_value = get_cycle_counter();
}

