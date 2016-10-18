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

#include <linux/file.h>
#include <linux/fs.h>
#include <linux/linkage.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <asm/mman.h>
#include <asm/registers.h>

asmlinkage int sys_mmap(unsigned long addr, size_t len,
			unsigned long prot, unsigned long flags,
			unsigned long fd, off_t off)
{
	int retval = -EINVAL;

	if (off & ~PAGE_MASK)
		goto out;

	retval = sys_mmap_pgoff(addr, len, prot, flags, fd, off >> PAGE_SHIFT);
out:
	return retval;
}

unsigned long straight_mmap2;

asmlinkage long sys_mmap2(unsigned long addr, size_t len,
			unsigned long prot, unsigned long flags,
			unsigned long fd, unsigned long pgoff)
{
	long ret = -EINVAL;

	if (!straight_mmap2)
		pgoff >>= PAGE_SHIFT-12;

	ret = sys_mmap_pgoff(addr, len, prot, flags, fd, pgoff);

	return ret;
}

