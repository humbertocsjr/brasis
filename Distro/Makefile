include ../.make.conf

all: 1440.img 720.img 1200.img cd.iso


clean:
	@-rm Temp/*
	@-rm *.img
	@-rm *.iso

cd.iso: 1440.img $(wildcard Temp/*.bin)  $(wildcard Temp/*.Nucleo) $(wildcard Temp/*.Imagem) $(wildcard Temp/*.Modulo)
	@echo -= Gerando imagem $(@) =-
	@cp 1440.img ISO/1440.img
	@mkisofs -o cd.iso -b 1440.img -hide 1440.img ISO

%.img: $(wildcard Temp/*.bin)  $(wildcard Temp/*.Nucleo) $(wildcard Temp/*.Imagem) $(wildcard Temp/*.Modulo)
	@echo -= Gerando imagem $(@) =-
	@dd if=/dev/zero of=$(@) bs=1024 count=$(patsubst %.img,%,$(@)) 2>/dev/null
	@minixfs mkfs $@ -1 -n 30 -s $(patsubst %.img,%,$(@))
	@dd if=Temp/MinixFS$(patsubst %.img,%,$(@)).bin of=$(@)  bs=1024 count=1 conv=notrunc 2>/dev/null
	@minixfs add $@ Temp/Estagio2.bin /BRASIS.Inicial
	@minixfs add $@ Temp/BRASIS.Nucleo /BRASIS.Nucleo
	@minixfs add $@ ../Inicial/Inicial.cfg /BRASIS.cfg
	@minixfs add $@ Temp/BRASIS.Imagem /BRASIS.Imagem
	@minixfs mkdir $@ /Inicial
	@minixfs add $@ Temp/MinixFS$(patsubst %.img,%,$(@)).bin /Inicial/MinixFS$(patsubst %.img,%,$(@)).bin
	@minixfs add $@ Temp/FAT12.bin /Inicial/FAT12.bin
	@minixfs add $@ Temp/Estagio2.bin /Inicial/Estagio2.bin
	@minixfs mkdir $@ /Sistema
	@minixfs add $@ Temp/MinixFS.Modulo /Sistema/MinixFS.Modulo
	@minixfs mkdir $@ /Programas
	@minixfs mkdir $@ /Perfis
	