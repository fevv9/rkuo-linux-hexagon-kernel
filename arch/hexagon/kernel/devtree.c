/*
 * Copyright (c) 2011-2015, The Linux Foundation. All rights reserved.
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

#include <linux/init.h>
#include <linux/libfdt.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/bootmem.h>
#include <linux/initrd.h>
#include <asm/platform.h>
#include <asm/prom.h>

void * __init early_init_dt_alloc_memory_arch(u64 size, u64 align)
{
	return alloc_bootmem_align(size, align);
}

#ifdef CONFIG_BLK_DEV_INITRD
void __init early_init_dt_setup_initrd_arch(u64 start,
					    u64 end)
{
	initrd_start = (unsigned long)__va(start);
	initrd_end = (unsigned long)__va(end);
	initrd_below_start_ok = 1;
}
#endif

/*
 * setup_machine_fdt - set up machine based on dtb passed to kernel
 * @dt_phys: physical address of dtb
 *
 * Sets up some of the earliest stuff -- setting the machine type,
 * pulling the command line options, and finding the memory.
 *
 * Since we're calling this from setup_arch, this is going to happen
 * very, very early.
 */
struct machine_desc * __init setup_machine_fdt(void *dt_phys)
{
	struct machine_desc *mdesc, *mdesc_best = NULL;
	unsigned int score, mdesc_score = ~1;
	unsigned long dt_root;

#ifdef CONFIG_DTB_BUILTIN
	initial_boot_params = dt_phys;
#else
	initial_boot_params = phys_to_virt(dt_phys);
#endif

	/* check device tree validity */
	if (fdt_magic(initial_boot_params) != OF_DT_HEADER)
		return NULL;

	dt_root = of_get_flat_dt_root();

	for_each_machine_desc(mdesc) {
		score = of_flat_dt_match(dt_root, mdesc->dt_compat);
		if (score > 0 && score < mdesc_score) {
			mdesc_best = mdesc;
			mdesc_score = score;
		}
	}

	if (!mdesc_best)
		panic("Unrecognized device tree\n");

	/*
	 * Sets the top level address and size cells which are stored
	 * in globals by the devtree infrastructure.
	*/
	of_scan_flat_dt(early_init_dt_scan_root, NULL);

	/*  Retrieve various information from the /chosen node  */
	of_scan_flat_dt(early_init_dt_scan_chosen, boot_command_line);

	/*  Todo:  Setup memory  */
	return mdesc_best;
}
