/*
 * Copyright (c) 2012,2015, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ASM_PROM_H
#define __ASM_PROM_H

#include <asm/setup.h>

#ifdef CONFIG_OF_FLATTREE
extern struct machine_desc *setup_machine_fdt(void *dt_phys);
#else
static inline struct machine_desc *setup_machine_fdt(void *dt_phys)
{
	return NULL;
}
#endif

#endif /* __ASM_PROM_H */
