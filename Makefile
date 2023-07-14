include .make.conf

all clean: Distro/Temp
	@$(MAKE) -C Inicial $@
	@$(MAKE) -C Nucleo $@
	@$(MAKE) -C Distro $@


run: all
	@dosbox -C "boot Distro/1440.img -l A"

debug_macos: all
	@bochs -q -f bochsmacosx

debug_linux: all
	@bochs -q -f bochslinux

Distro/Temp:
	@mkdir Distro/Temp