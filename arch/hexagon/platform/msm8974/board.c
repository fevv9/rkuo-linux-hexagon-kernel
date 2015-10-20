/*
 * Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <asm/dma.h>
#include <asm/irq.h>
#include <asm/platform.h>

static const char *msm8974_dt_compat[] __initconst = {
	"qcom,msm8974",
	NULL
};

static int __init msm8974_init(void)
{
	if (strcmp(mdesc->name, "msm8974"))
		return 0;

	hexagon_dma_init();

	of_platform_populate(of_find_node_by_path("/soc"),
		of_default_bus_match_table, NULL, NULL);

	return 0;
}
core_initcall(msm8974_init);

void setup_arch_msm8974(void)
{
	pr_info("Platform:  MDP 8974\n");
	bootmem_lastpg = PFN_DOWN(1<<28);
}

MACHINE_START(MSM8974_FLUID, "msm8974")
	.setup_arch_machine = setup_arch_msm8974,
	.dt_compat = msm8974_dt_compat
MACHINE_END
