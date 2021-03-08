#ifndef _IO_H_
#define _IO_H_

#include <cmsis_device.h>

#define ARRAY_SIZE(x)	(int)(sizeof(x) / sizeof((x)[0]))

#define readl(_a)		(*(volatile uint32_t *)(_a))
#define writel(_v, _a)	(*(volatile uint32_t *)(_a) = (_v))

#define _maskbits(_bp, _bw)                	(((1 << _bw) - 1) << _bp)
#define _getsftbits(_in, _bp, _bw)         	(((_in) & _maskbits(_bp, _bw)) >> _bp)
#define _clrsetbits(_in, _bp, _bw, _val)	(((_in) & (uint32_t)~_maskbits(_bp, _bw)) | (uint32_t)(((_val) << _bp) & _maskbits(_bp, _bw)))
#define _writebits(_addr, _bp, _bw, _val)	writel(_clrsetbits(readl(_addr), _bp, _bw, _val), _addr)
#define _readbits(_addr, _bp, _bw)			_getsftbits(readl(_addr), _bp, _bw)

#endif

