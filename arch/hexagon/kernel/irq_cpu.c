/*
 * First-level interrupt controller model for Hexagon.
 *
 * Copyright (c) 2010-2011, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <linux/interrupt.h>
#include <linux/irqchip.h>
#include <linux/irqdomain.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <asm/irq.h>
#include <asm/hexagon_vm.h>

static void mask_irq(struct irq_data *data)
{
	__vmintop_locdis((long) data->irq);
}

static void mask_irq_num(unsigned int irq)
{
	__vmintop_locdis((long) irq);
}

static void unmask_irq(struct irq_data *data)
{
	__vmintop_locen((long) data->irq);
}

/*  This is actually all we need for handle_fasteoi_irq  */
static void eoi_irq(struct irq_data *data)
{
	__vmintop_globen((long) data->irq);
}

void __init init_IRQ(void)
{
	irqchip_init();
}
