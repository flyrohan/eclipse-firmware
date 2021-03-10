#ifndef _INIT_H_
#define _INIT_H_

typedef void (*initcall_t)(void);
#define SYSTEM_INIT(fn) \
	initcall_t _init_##fn __attribute__ ((section(".init_array."#fn""))) = fn
#endif
