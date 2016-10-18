/*
 * Copyright (c) 2013-2016, The Linux Foundation. All rights reserved.
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
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <asm/irq.h>
#include <asm/hexagon_vm.h>
#include <asm/platform.h>

static const char *msm8996_dt_compat[] __initconst = {
	"qcom,msm8996",
	NULL
};

static struct platform_device *devices[] __initdata = {
};

const struct of_device_id platform_of_irq_matches[] __initconst  = {
	{ .compatible = "qcom,h2-pic", .data = hexagon_pic_of_init, },
	{},
};


static int __init msm8996_init(void)
{
	if (strcmp(mdesc->name, "msm8996"))
		return 0;

	platform_add_devices(devices, ARRAY_SIZE(devices));

	of_platform_populate(of_find_node_by_path("/soc"),
		of_default_bus_match_table, NULL, NULL);

	return 0;
}
arch_initcall(msm8996_init);

void setup_arch_msm8996(void)
{
	bootmem_lastpg = PFN_DOWN(1<<26);

}

MACHINE_START(MSM8996, "msm8996")
	.setup_arch_machine = setup_arch_msm8996,
	.dt_compat = msm8996_dt_compat
MACHINE_END
