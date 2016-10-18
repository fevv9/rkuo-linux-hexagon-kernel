
ifdef CONFIG_HEXAGON_MSM8974
dtb-${CONFIG_HEXAGON_MSM8974}	+= msm8974.dtb
obj-${CONFIG_HEXAGON_MSM8974}	+= msm8974.dtb.o
endif

dtb-${CONFIG_HEXAGON_MSM8996}	+= msm8996.dtb
obj-${CONFIG_HEXAGON_MSM8996}	+= msm8996.dtb.o

targets += dtbs
targets += $(dtb-y)

dtbs: $(addprefix $(obj)/, $(dtb-y))
