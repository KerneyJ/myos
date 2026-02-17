#ifndef COMMON_MM_H
#define COMMON_MM_H

#include <stddef.h>
#include <stdint.h>

#ifdef CONFIG_x86_64
#include "arch/x86_64/early_mem.h"
#endif

#define PG_BIG 0x80
#define PG_PRESENT 0x01
#define PG_WRITABLE 0x02

#endif
