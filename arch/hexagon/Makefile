#  Makefile for the Hexagon arch

KBUILD_DEFCONFIG = msm8974_defconfig

# Do not use GP-relative jumps
KBUILD_CFLAGS += -G0
LDFLAGS_vmlinux += -G0

# Do not use single-byte enums; these will overflow.
KBUILD_CFLAGS += -fno-short-enums

# Modules must use either long-calls, or use pic/plt.
# Use long-calls for now, it's easier.  And faster.
# CFLAGS_MODULE += -fPIC
# LDFLAGS_MODULE += -shared
CFLAGS_MODULE += -mlong-calls

cflags-y += $(call cc-option,-mv${CONFIG_HEXAGON_ARCH_VERSION})
aflags-y += $(call cc-option,-mv${CONFIG_HEXAGON_ARCH_VERSION})
ldflags-y += $(call cc-option,-mv${CONFIG_HEXAGON_ARCH_VERSION})

KBUILD_CFLAGS += $(cflags-y)
KBUILD_AFLAGS += $(aflags-y)

#  no KBUILD_LDFLAGS?
LDFLAGS += $(ldflags-y)

# Thread-info register will be r19.  This value is not configureable;
# it is hard-coded in several files.
TIR_NAME := r19
KBUILD_CFLAGS += -ffixed-$(TIR_NAME) -DTHREADINFO_REG=$(TIR_NAME) -D__linux__
KBUILD_AFLAGS += -DTHREADINFO_REG=$(TIR_NAME)

LIBGCC := $(shell $(CC) $(KBUILD_CFLAGS) -print-libgcc-file-name)
libs-y += $(LIBGCC)

head-y := arch/hexagon/kernel/head.o

core-y += arch/hexagon/kernel/ \
	arch/hexagon/mm/ \
	arch/hexagon/lib/

core-$(CONFIG_HEXAGON_MSM8974)	+= arch/hexagon/platform/msm8974/
core-$(CONFIG_HEXAGON_MSM8996)	+= arch/hexagon/platform/msm8996/

#  Devtree blob targets
boot := arch/hexagon/boot
core-$(CONFIG_DTB_BUILTIN) += arch/hexagon/boot/dts/

PHONY += dtbs
dtbs: scripts
	$(Q)$(MAKE) $(build)=$(boot)/dts dtbs
