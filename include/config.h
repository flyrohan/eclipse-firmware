#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <cmsis_device.h>

//////////////////////////////////////////////////////////////////////////////////////
// App Enable
//////////////////////////////////////////////////////////////////////////////////////
#define LOOP_ENABLED

//////////////////////////////////////////////////////////////////////////////////////
// Module Enable
//////////////////////////////////////////////////////////////////////////////////////
#define TIMER_ENABLED		/* replace SYSTICK_ENABLED */
#define HAL_INIT
#define REMAP_ENABLED
#define PLL_ENABLED
#define CLI_ENABLED
#define CMD_ENABLED
#define CONSOLE_ENABLED
#define CONSOLE_UART_ENABLED
#define GPIO_ENABLED
#define DMIPS_ENABLED

//////////////////////////////////////////////////////////////////////////////////////
// System Configure
//////////////////////////////////////////////////////////////////////////////////////
#define SYSTEM_CLOCK		48000000
#define SYSTEM_TICK_HZ		1000
#define UART_CHANNEL
#define SYSTICK_IRQ_MODE	(1)
#define TIMER_IRQ_MODE		(0)
#endif
