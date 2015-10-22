/*
 * Copyright (c) 2010-2015, The Linux Foundation. All rights reserved.
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

#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/percpu.h>
#include <asm/irq.h>
#include <asm/hexagon_vm.h>

#include "irqchip.h"

static void mask_irq_num(unsigned int hwirq)
{
	__vmintop_locdis((long) hwirq);
}

static void mask_irq(struct irq_data *data)
{
	mask_irq_num(data->hwirq);
}

static void unmask_irq(struct irq_data *data)
{
	__vmintop_locen((long) data->hwirq);
}

static void eoi_irq(struct irq_data *data)
{
	__vmintop_globen((long) data->hwirq);
}

static struct irq_chip hexagon_irq_chip = {
	.name		= "HEXAGON",
	.irq_mask	= mask_irq,
	.irq_unmask	= unmask_irq,
	.irq_eoi	= eoi_irq
};

struct irq_domain *hexagon_irq_domain;

int __init hexagon_pic_of_init(struct device_node *node,
			  struct device_node *parent)
{
	int irq;	/*  "hardware" IRQ's  */
	int virq;	/*  linux IRQ's  */

	hexagon_irq_domain = irq_domain_add_linear(node, HEXAGON_CPUINTS,
			&irq_domain_simple_ops, &hexagon_irq_chip);


	if (!hexagon_irq_domain) {
		WARN(1, "Cannot allocate irq_domain\n");
		return -ENOMEM;
	}

	irq_set_default_host(hexagon_irq_domain);

	/**
	 * The first-level interrupt controller is wrapped by the VM, which
	 * virtualizes the interrupt controller for us.  It provides a very
	 * simple, fast & efficient API, and so the fasteoi handler is
	 * appropriate for this case.
	 */

	for (irq = 0; irq < HEXAGON_CPUINTS; irq++) {
		mask_irq_num(irq);
		__vmintop_globen((long) irq);
		virq = irq_create_mapping(hexagon_irq_domain, irq);
		irq_set_chip_and_handler(virq, &hexagon_irq_chip,
						 handle_fasteoi_irq);
	}

	return 0;
}

IRQCHIP_DECLARE(hexagon_h2_pic, "qcom,h2-pic", hexagon_pic_of_init);
