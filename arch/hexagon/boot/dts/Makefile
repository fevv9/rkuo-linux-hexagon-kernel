
ifdef CONFIG_HEXAGON_MSM8974
dtb-${CONFIG_HEXAGON_MSM8974}	+= msm8974.dtb
obj-${CONFIG_HEXAGON_MSM8974}	+= msm8974.dtb.o
endif

targets += dtbs
targets += $(dtb-y)

dtbs: $(addprefix $(obj)/, $(dtb-y))
