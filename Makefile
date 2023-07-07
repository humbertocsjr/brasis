include .make.conf

all clean: Distro/Temp
	@$(MAKE) -C Inicial $@
	@$(MAKE) -C Nucleo $@
	@$(MAKE) -C Distro $@


run: all
	@dosbox -C "boot Distro/1440.img -l A"

Distro/Temp:
	@mkdir Distro/Temp