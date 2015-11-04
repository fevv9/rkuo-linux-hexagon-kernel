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

#include <linux/init.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include <asm/timer-regs.h>
#include <asm/platform.h>
#include <asm/hexagon_vm.h>

cycles_t        hvm_timer_freq;

static cycle_t timer_get_cycles(struct clocksource *cs)
{
	return (cycle_t) __vmtimerop(gettime, 0, 0);
}

static struct clocksource hexagon_clocksource = {
	.name		= "HVM timer",
	.rating		= 250,
	.read		= timer_get_cycles,
	.mask		= CLOCKSOURCE_MASK(64),
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

static int set_next_event(unsigned long delta, struct clock_event_device *evt)
{
	unsigned long long ret;

	/*  this version of the timer should only be set by CPU 0  */
	if (smp_processor_id() != 0)
		return 0;

	/*
	 * ret == 0 means that the requested time has already passed, so we were
	 * preempted on the gettime trap (which returns the time it was
	 * called, not the time it completes.  So we have to try again
	 */
	do {
		ret = __vmtimerop(deltatimeout, 0,
				  clockevent_delta2ns(delta, evt));
	} while (ret == 0);

	return 0;
}

/*
 * Sets the mode (periodic, shutdown, oneshot, etc) of a timer.
 */
static void set_mode(enum clock_event_mode mode,
	struct clock_event_device *evt)
{
	switch (mode) {
	case CLOCK_EVT_MODE_SHUTDOWN:
		/* XXX implement me */
	default:
		break;
	}
}

#ifdef CONFIG_SMP
/*  Broadcast mechanism  */
static void broadcast(const struct cpumask *mask)
{
	send_ipi(mask, IPI_TIMER);
}
#endif

static struct clock_event_device hexagon_clockevent_dev = {
	.name		= "clockevent",
	.features	= CLOCK_EVT_FEAT_ONESHOT,
	.rating		= 400,
	.set_next_event = set_next_event,
	.set_mode	= set_mode,
#ifdef CONFIG_SMP
	.broadcast	= broadcast,
#endif
};

#ifdef CONFIG_SMP
static DEFINE_PER_CPU(struct clock_event_device, clock_events);

void setup_percpu_clockdev(void)
{
	int cpu = smp_processor_id();
	struct clock_event_device *ce_dev = &hexagon_clockevent_dev;
	struct clock_event_device *dummy_clock_dev =
		&per_cpu(clock_events, cpu);

	memcpy(dummy_clock_dev, ce_dev, sizeof(*dummy_clock_dev));
	INIT_LIST_HEAD(&dummy_clock_dev->list);

	dummy_clock_dev->features = CLOCK_EVT_FEAT_DUMMY;
	dummy_clock_dev->cpumask = cpumask_of(cpu);
	dummy_clock_dev->mode = CLOCK_EVT_MODE_UNUSED;

	clockevents_register_device(dummy_clock_dev);
}

/*  Called from smp.c for each CPU's timer ipi call  */
void ipi_timer(void)
{
	int cpu = smp_processor_id();
	struct clock_event_device *ce_dev = &per_cpu(clock_events, cpu);

	ce_dev->event_handler(ce_dev);
}
#endif /* CONFIG_SMP */

static irqreturn_t timer_interrupt(int irq, void *devid)
{
	struct clock_event_device *ce_dev = &hexagon_clockevent_dev;

	ce_dev->event_handler(ce_dev);

	return IRQ_HANDLED;
}

/*  This should also be pulled from devtree  */
static struct irqaction rtos_timer_intdesc = {
	.handler = timer_interrupt,
	.flags = IRQF_TIMER | IRQF_TRIGGER_RISING,
	.name = "rtos_timer"
};

/*
 * time_init_deferred - called by start_kernel to set up timer/clock source
 *
 * Install the IRQ handler for the clock, setup timers.
 * This is done late, as that way, we can use ioremap().
 *
 * This runs just before the delay loop is calibrated, and
 * is used for delay calibration.
 */
void __init time_init(void)
{

	struct clock_event_device *ce_dev = &hexagon_clockevent_dev;
	struct device_node *dn;

	ce_dev->cpumask = cpu_all_mask;

	hvm_timer_freq = (cycles_t)__vmtimerop(getfreq, 0, 0);

	dn = of_find_compatible_node(NULL, NULL, "qcom,h2-timer");
	if (dn)
		ce_dev->irq = irq_of_parse_and_map(dn, 0);
	else
		panic("%s could not find device\n", __func__);

	ce_dev->cpumask = cpu_all_mask;

	clocksource_register_khz(&hexagon_clocksource, hvm_timer_freq / 1000);

	/*
	 * Last arg is some guaranteed seconds for which the conversion will
	 * work without overflow.
	 */
	clockevents_calc_mult_shift(ce_dev, hvm_timer_freq, 4);

	ce_dev->max_delta_ns = clockevent_delta2ns(0x7fffffff, ce_dev);
	ce_dev->min_delta_ns = clockevent_delta2ns(0xf, ce_dev);

#ifdef CONFIG_SMP
	setup_percpu_clockdev();
#endif

	clockevents_register_device(ce_dev);
	setup_irq(ce_dev->irq, &rtos_timer_intdesc);
}


void __delay(unsigned long loops)
{
	asm volatile(
		"loop0(1f, %0);"
		"1:	{ nop; } :endloop0"
		:
		: "r" (loops)
	);
}
EXPORT_SYMBOL(__delay);


static long long fudgefactor = 600;  /* Maybe lower if kernel optimized. */

void __udelay(unsigned long usecs)
{
	unsigned long long start = __vmtimerop(gettime, 0, 0);
	unsigned long long finish;

	finish = (hvm_timer_freq * usecs / (1000 * 1000)) - fudgefactor;

	while ((__vmtimerop(gettime, 0, 0) - start) < finish)
		cpu_relax();
}
EXPORT_SYMBOL(__udelay);
